#ifndef ZOMBIELAND_ACTION_ATTACK_H
#define ZOMBIELAND_ACTION_ATTACK_H

#include <Isilme.h>
#include "ZombieLand/Include/Definitions.h"
#include "ZombieLand/Behaviour/Creature.h"

namespace action
{
	class Attack : public Action
	{
	public:
		Attack();
		virtual ~Attack();

		virtual void OnStart();

		void	SetSound(std::string sound);
	private:
		HEFFECT	mSound;
		float mDamage;
		float mDistance;
		float mWidth;
	};
};

#endif