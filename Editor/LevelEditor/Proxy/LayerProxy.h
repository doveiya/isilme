#pragma once

#include "../Stdafx.h"
#include "../SharedCLIPtr.h"

using namespace Common;
using namespace System;
using namespace System::Collections::ObjectModel;

namespace LevelEditor
{
	namespace Proxy
	{

		ref class EntityProxy;

		[System::ComponentModel::TypeConverter(System::ComponentModel::ExpandableObjectConverter::typeid)]
		public ref class GridProxy
		{
		public:
			GridProxy()
			{
				Step = 1.0f;
			}
			GridProxy(float step)
			{
				Step = step;
			}

			property float Step
			{
				float get()
				{
					return mStep;
				}
				void set(float value)
				{
					mStep = value;
				}
			}
		private:
			float mStep;
		};

		public ref class LayerProxy : ProxyObject
		{
		public:
			LayerProxy(LayerPtr layer);


			/// �������� �������� �� ����
			///
			/// @param entity	����������� ������
			void AddEntity(EntityProxy^ entity);


			/// ������� ������ �� ����
			///
			/// @param [in,out]	entity	If non-null, the entity.
			void RemoveEntity(EntityProxy^ entity);

			/// ���������� ��������� �������� ����
			///
			/// @return	The entities.
			property ObservableCollection<EntityProxy^>^ Entities
			{
				ObservableCollection<EntityProxy^>^ get();
			}

			property String^ Name
			{
				String^ get();
				void set(String^ value);
			}

			property bool Visible
			{
				bool get();
				void set(bool value);
			}

			property LevelProxy^ Level
			{
				LevelProxy^ get();
			}

			[System::ComponentModel::TypeConverter(System::ComponentModel::ExpandableObjectConverter::typeid)]
			property GridProxy^ Grid
			{
				GridProxy^ get();
				void set(GridProxy^ value);
			}
		internal:
			LevelProxy^ mLevel;

			GridProxy^ mGrid;
		
			SharedCLIPtr<Layer>* mLayer;
		private:
			ObservableCollection<EntityProxy^>^ mEntities;
		};
	}
}