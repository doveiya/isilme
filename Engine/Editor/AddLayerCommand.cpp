#include "IsilmePCH.h"
#include "AddLayerCommand.h"
#include "Core/Layer.h"
#include "Core/Level.h"

namespace editor
{
	AddLayerCommand::AddLayerCommand(LevelPtr level, LayerPtr layer)
	{
		mLevel = level;
		mLayer = layer;
	}

	AddLayerCommand::~AddLayerCommand()
	{
	}

	void AddLayerCommand::Execute()
	{
		mLevel->AddLayer(mLayer);
	}

	void AddLayerCommand::Unexecute()
	{
		mLevel->RemoveLayer(mLayer);
	}
};