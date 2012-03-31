﻿#ifndef ISILME_SERIALISATION_CONVERSATIONLOADER_H
#define ISILME_SERIALISATION_CONVERSATIONLOADER_H

#include "../Definitions.h"
#include "EntryLoader.h"
#include "Engine/Story/Conversation.h"
#include "Engine/Story/Phrase.h"

namespace serialisation
{
	class ISILME_API ConversationLoader : public EntryLoader
	{
	public:
		ConversationLoader();
		virtual ~ConversationLoader();

		virtual EntryPtr LoadEntry(std::string filename);
	protected:
		virtual story::ConversationPtr LoadConversation(std::string fileName);
		virtual story::ConversationPtr LoadConversation(TiXmlElement* element);
		virtual story::PhrasePtr LoadPhrase(TiXmlElement* element);

		story::PhrasePtr GetPhraseByRef(int refValue);
	private:
		typedef std::map<int, story::PhrasePtr> IDPhraseMap;
		typedef std::map<story::PhrasePtr, int> PhraseRefMap;
		IDPhraseMap mPhraseMap;
		PhraseRefMap mRefMap;
	};
}
#endif