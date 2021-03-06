#include "WmaDecoder.h"

#pragma comment(lib, "amstrmid")

#define WMA_BUFFER_SIZE 2048

CDecompressWma::CDecompressWma(WAVEFORMATEX* pcm_format, bool& flag,
	CAbstractSoundFile* a)
	: CAbstractDecompressor(pcm_format, flag, a)
{
	flag = false;

	HRESULT hr;
	IAMMultiMediaStream* AMMStream_ptr;

	// ������������� COM
	CoInitialize(NULL);

	SourceData->free_file_resource();

	WCHAR name[1024];
	MultiByteToWideChar(CP_ACP,
		0,
		SourceData->get_file_name(),
		-1,
		name,
		sizeof(name) / sizeof(name[0]));

	CoCreateInstance(CLSID_AMMultiMediaStream,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IAMMultiMediaStream,
		(void * *) &AMMStream_ptr);

	hr = AMMStream_ptr->Initialize(STREAMTYPE_READ, AMMSF_NOGRAPHTHREAD, NULL);
	if (hr != 0)
		return;

	hr = AMMStream_ptr->AddMediaStream(NULL, & MSPID_PrimaryAudio, 0, NULL);
	if (hr != 0)
		return;

	hr = AMMStream_ptr->OpenFile(name, AMMSF_RUN);
	if (hr != 0)
		return;

	MMStream_ptr = AMMStream_ptr;

	// �� ��������� ������� ������� ��������� �������� �����
	MMStream_ptr->GetMediaStream(MSPID_PrimaryAudio, & MStream_ptr);

	// �� ������ �� ������ ������� ����� � ����� �������
	MStream_ptr->QueryInterface(IID_IAudioMediaStream,
				 	(void * *) &AudioStream_ptr);

	// ������� ������ ����� ������
	AudioStream_ptr->GetFormat(&WmaFormat);

	// �������� ����������� ������ ��� ����� �������
	CoCreateInstance(CLSID_AMAudioData,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IAudioData,
		(void * *) &AudioData_ptr);

	// ����������� ������ ��� Direct X (����� Direct X �� ������ ������� �����)
	pcm_format->wFormatTag = 1;
	pcm_format->wBitsPerSample = 16;
	pcm_format->nSamplesPerSec = WmaFormat.nSamplesPerSec;
	pcm_format->nChannels = WmaFormat.nChannels;
	pcm_format->nBlockAlign = (pcm_format->nChannels * pcm_format->wBitsPerSample) >>
		3;
	pcm_format->nAvgBytesPerSec = pcm_format->nBlockAlign * pcm_format->nSamplesPerSec;

	// ��������� ������ ��� ������������� �����
#if AGSS_USE_MALLOC
	temp_wma_buffer = (WORD *)
		malloc(WMA_BUFFER_SIZE * WmaFormat.nBlockAlign);
#else
	temp_wma_buffer = (WORD *) GlobalAlloc(GPTR,
							   	WMA_BUFFER_SIZE * WmaFormat.nBlockAlign);
#endif

	if (!temp_wma_buffer)
		return;

	// ��� ������ �������
	flag = true;
}

CDecompressWma::~CDecompressWma()
{
	if (MMStream_ptr) {
		MMStream_ptr->SetState(STREAMSTATE_STOP);
	}
	if (AudioData_ptr) {
		AudioData_ptr->Release();
		AudioData_ptr = 0;
	}

	if (AudioStream_ptr) {
		AudioStream_ptr->Release();
		AudioStream_ptr = 0;
	}

	if (MStream_ptr) {
		MStream_ptr->Release();
		MStream_ptr = 0;
	}

	if (MMStream_ptr) {
		MMStream_ptr->Release();
		MMStream_ptr = 0;
	}
	if (temp_wma_buffer) {
#if AGSS_USE_MALLOC
		free(temp_wma_buffer);
#else
		GlobalFree(temp_wma_buffer);
#endif
		temp_wma_buffer = 0;
	}

	CoUninitialize();
}


// ��������� ���������� ������� � �����
DWORD CDecompressWma::GetStereoSamples(void* buffer, DWORD start,
	DWORD length, bool loop)
{
	HRESULT hr;
	DWORD NeedSamples, Temp;
	IAudioStreamSample* Sample;
	//	STREAM_TIME StartTime, EndTime, CurTime;

	LONGLONG temp = start;
	LONGLONG a = WmaFormat.nSamplesPerSec;

	DWORD NeedSkip = start;

	NeedSamples = start;


	LONGLONG s = temp * 10000000 / a;
	if (!start) {
		hr = MMStream_ptr->SetState(STREAMSTATE_STOP);
		hr = MMStream_ptr->Seek(s);
		hr = MMStream_ptr->SetState(STREAMSTATE_RUN);
	}

	if (WmaFormat.nChannels == 2) {
		// ��������� ������ ������
		// ��������� ������ ������������ ������
		hr = AudioData_ptr->SetBuffer(length * WmaFormat.nBlockAlign,
								(BYTE *) buffer,
								0);
		if (hr != S_OK)
			return 0;
		// ��������� ������ ���������� ������
		hr = AudioData_ptr->SetFormat(&WmaFormat);
		if (hr != S_OK)
			return 0;
		// ���������� ������ 
		hr = AudioStream_ptr->CreateSample(AudioData_ptr, 0, & Sample);
		if (hr != S_OK)
			return 0;

		// ��������� � ����� ������
		hr = Sample->Update(SSUPDATE_ASYNC, 0, 0, 0);
		hr = Sample->CompletionStatus(COMPSTAT_WAIT, INFINITE);

		//		if (hr == MS_S_PENDING) {
		//			hr = Sample->CompletionStatus(COMPSTAT_WAIT,INFINITE);
		//		}
		if (FAILED(hr) || MS_S_ENDOFSTREAM == hr) {
			return 0;
		}

		// ������ ������� ������ � ����� ���������
		hr = AudioData_ptr->GetInfo(NULL, NULL, & Temp);
		if (hr != S_OK)
			return 0;
		// ��������� ��� ������ ?
		if (Temp != length * 4)
			return 0;
		// �������� ��������� ������������ ������
		Sample->Release();
	} else {
		// ��������� ���� ������ � ��������������� � ������
		// ���������� ����������
		WORD* dst = (WORD*) buffer;
		NeedSamples = length;

		while (NeedSamples) {
			Temp = NeedSamples;
			if (Temp > WMA_BUFFER_SIZE)
				Temp = WMA_BUFFER_SIZE;
			// ��������� ������ ������������ ������
			hr = AudioData_ptr->SetBuffer(Temp * WmaFormat.nBlockAlign,
									(BYTE *) temp_wma_buffer,
									0);
			if (hr != S_OK)
				return 0;
			// ��������� ������ ���������� ������
			hr = AudioData_ptr->SetFormat(&WmaFormat);
			if (hr != S_OK)
				return 0;
			// ���������� ������ 
			hr = AudioStream_ptr->CreateSample(AudioData_ptr, 0, & Sample);
			if (hr != S_OK)
				return 0;

			//			HANDLE hEvent = CreateEvent(FALSE, NULL, NULL, FALSE);
			//			hr = Sample->Update(0, hEvent, NULL, 0);
			// ��������� � ����� ������

			hr = Sample->Update(SSUPDATE_ASYNC, 0, 0, 0);   	   			
			hr = Sample->CompletionStatus(COMPSTAT_WAIT, INFINITE);
			if (FAILED(hr) || MS_S_ENDOFSTREAM == hr) {
				return 0;
			}

			hr = AudioData_ptr->GetInfo(NULL, NULL, & Temp);
			if (hr != S_OK)
				return 0;

			Temp /= WmaFormat.nBlockAlign;

			NeedSamples -= Temp;
			WORD* src = temp_wma_buffer;
			do {
				WORD a =* src++;
				*dst++ = a;
				*dst++ = a;
			} while (--Temp);
			Sample->Release();
		}
	}
	return length * 4;
}

// ��������� ���������� ������� � �����
DWORD CDecompressWma::GetMonoSamples(void* buffer, DWORD start, DWORD length,
	bool loop)
{
	HRESULT hr;
	DWORD NeedSamples, Temp;
	IAudioStreamSample* Sample;

	if (WmaFormat.nChannels == 1) {
		// ��������� ���� ������
		// ��������� ������ ������������ ������
		hr = AudioData_ptr->SetBuffer(length * WmaFormat.nBlockAlign,
								(BYTE *) buffer,
								0);
		if (hr != S_OK)
			return 0;
		// ��������� ������ ���������� ������
		hr = AudioData_ptr->SetFormat(&WmaFormat);
		if (hr != S_OK)
			return 0;
		// ���������� ������ 
		hr = AudioStream_ptr->CreateSample(AudioData_ptr, 0, & Sample);
		if (hr != S_OK)
			return 0;
		// ��������� � ����� ������
		hr = Sample->Update(SSUPDATE_ASYNC, 0, 0, 0);
		if (FAILED(hr) || MS_S_ENDOFSTREAM == hr) {
			return 0;
		}
		// ������ ������� ������ � ����� ���������
		hr = AudioData_ptr->GetInfo(NULL, NULL, & Temp);
		if (hr != S_OK)
			return 0;
		// ��������� ��� ������ ?
		if (Temp != length * 2)
			return 0;
		// �������� ��������� ������������ ������
		Sample->Release();
	} else {
		// ��������� ������ ������ � ��������������� � ����
		// ���������� ����������
		short* dst = (short*) buffer;
		NeedSamples = length;

		while (NeedSamples) {
			Temp = NeedSamples;
			if (Temp > WMA_BUFFER_SIZE)
				Temp = WMA_BUFFER_SIZE;
			// ��������� ������ ������������ ������
			hr = AudioData_ptr->SetBuffer(Temp * WmaFormat.nBlockAlign,
									(BYTE *) temp_wma_buffer,
									0);
			if (hr != S_OK)
				return 0;
			// ��������� ������ ���������� ������
			hr = AudioData_ptr->SetFormat(&WmaFormat);
			if (hr != S_OK)
				return 0;
			// ���������� ������ 
			hr = AudioStream_ptr->CreateSample(AudioData_ptr, 0, & Sample);
			if (hr != S_OK)
				return 0;
			// ��������� � ����� ������
			hr = Sample->Update(SSUPDATE_ASYNC, 0, 0, 0);
			if (FAILED(hr) || MS_S_ENDOFSTREAM == hr) {
				return 0;
			}

			hr = AudioData_ptr->GetInfo(NULL, NULL, & Temp);
			if (hr != S_OK)
				return 0;

			Temp /= WmaFormat.nBlockAlign;

			NeedSamples -= Temp;
			short* src = (short*) temp_wma_buffer;
			do {
				int val = (int) (*src++ + *src++) / 2;
				if (val > 32767)
					val = 32767;
				else if (val < -32768)
					val = -32768;
				*dst++ = (WORD) val;
			} while (--Temp);

			Sample->Release();
		}
	}
	return length * 2;
}

// ��������� ���������� ������� � �����
DWORD CDecompressWma::GetStereoMute(void* buffer, DWORD length)
{
	length <<= 2;
	memset(buffer, 0, length);
	return length;
}

// ��������� ���������� ������� � �����
DWORD CDecompressWma::GetMonoMute(void* buffer, DWORD length)
{
	length <<= 1;
	memset(buffer, 0, length);
	return length;
}

// ��������� ���������� ������� � �����
DWORD CDecompressWma::GetSamplesInFile(void)
{
	STREAM_TIME a;
	MMStream_ptr->GetDuration(&a);
	return (DWORD) ((a * WmaFormat.nSamplesPerSec) / 10000000);
}

// ��������� ���������� ���� � ����� ���� �����
DWORD CDecompressWma::GetRealMonoDataSize(void)
{
	return GetSamplesInFile() * 2;
}

// ��������� ���������� ���� � ����� ������ �����
DWORD CDecompressWma::GetRealStereoDataSize(void)
{
	return GetSamplesInFile() * 4;
}
