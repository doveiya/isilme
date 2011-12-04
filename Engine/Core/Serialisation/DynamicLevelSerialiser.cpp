#include "IsilmePCH.h"
#include "DynamicLevelSerialiser.h"
#include "LevelSerialiser.h"
#include "Layer.h"
#include "Level.h"
#include "Entity.h"
#include "Behaviour.h"
#include "FactoryManager.h"

namespace serialisation
{
	DynamicLevelSerialiser::DynamicLevelSerialiser()
	{
	
	}

	DynamicLevelSerialiser::~DynamicLevelSerialiser()
	{
	
	}

	void DynamicLevelSerialiser::Serialise(LevelPtr level, std::string fileName)
	{

	}

	TiXmlElement* DynamicLevelSerialiser::Serialise( LevelPtr level )
	{
		TiXmlElement* levelElement = new TiXmlElement("Level");

		for (int i = 0; i < level->GetLayersCount(); ++i)
		{
			levelElement->InsertEndChild(*SerialiseLayer(level->GetLayer(i)));
		}
		return levelElement;
	}

	TiXmlElement* DynamicLevelSerialiser::SerialiseEntity( EntityPtr entity )
	{
		TiXmlElement* element = new TiXmlElement("Entity");

		// ��������� ����� ������
		element->SetAttribute("ID", entity->GetName().c_str());
		element->SetAttribute("Type", entity->GetType().c_str());

		// ��������� ���������, ����, �������
		char position[256];
		sprintf(position, "%d, %d", entity->GetPosition().x, entity->GetPosition().y);
		element->SetAttribute("Position", position);
		element->SetAttribute("Angle", entity->GetAngle());
		element->SetAttribute("Scale", entity->GetScale());

		return element;
	}

	TiXmlElement* DynamicLevelSerialiser::SerialiseWorld()
	{
		TiXmlElement* elemet = new TiXmlElement("Data");

		FactoryPtr factory = FactoryManager::GetSingleton();

		for (std::map<std::string, LevelPtr>::iterator lIt = factory->mLevels.begin(); lIt != factory->mLevels.end(); ++lIt)
		{
			LevelPtr level = lIt->second;
			elemet->InsertEndChild(*Serialise(level));
		}

		return elemet;
	}

	TiXmlElement* DynamicLevelSerialiser::SerialiseLayer( LayerPtr layer )
	{
		TiXmlElement* layerElement = new TiXmlElement("Layer");

		for (int i = 0; i < layer->Size(); ++i)
		{
			layerElement->InsertEndChild(*SerialiseEntity(layer->GetEntity(i)));
		}

		return layerElement;
	}

};