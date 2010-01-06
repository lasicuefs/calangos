#ifndef PREDATOR_H

#define PREDATOR_H



#include "animatedObjetoJogo.h"

#include "animal.h"



class Predator : public Animal {
public:

	Predator();
	Predator(PT(AnimatedObjetoJogo) base_object);

	~Predator();

	static void load_predators();

	virtual void act();

private:
	void pursuit();
	void bite();
};



#endif

