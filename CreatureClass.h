#ifndef CREATURECLASS_H
#define CREATURECLASS_H
#include "Buff.h"

class Creature
{
public:
	Creature();
	~Creature();

private:
	BuffBase* m_Buff;
protected:
	virtual void CreatureAttack();

};

Creature::Creature()
{
}

Creature::~Creature()
{
}

#endif
