#ifndef FROG_H
#define FROG_H

#include "animal.h"

class Frog : public Animal {
public:
	Frog(NodePath node);

	void change_sector(PT(Setor) new_sector);
	static void load_frogs();
	//virtual void act();
};

#endif
