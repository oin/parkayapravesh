#include <cassert>
#include "entrees.h"
#include <cmath>

const doigt doigt::aucun(true);

bool doigt::operator==(doigt& d) {
	if(existe() == d.existe()) return true;
	return d.x() == x() && d.y() == y();
}

double doigt::distance(doigt& doigt1_, doigt& doigt2_) {
	return sqrt(pow(doigt2_.x() - doigt1_.x(), 2) + pow(doigt2_.y() - doigt1_.y(), 2));
}

std::ostream & operator<<(std::ostream & out, doigt& d) {
	if(d.existe())
		out << "(" << d.id() << ": [" << d.x() << "," << d.y() << "]; " << d.vitesse() << "; " << d.acceleration() << ")";
	else
		out << "(aucun)";
	return out;
}

int luigi::nb_doigts() {
	if(!doigt2_.existe())
		if(!doigt1_.existe())
		return 0;
	return 1;
	return 2;
}

void luigi::set_doigt(doigt d) {
	if(doigt2_.id() == d.id())
		doigt2_ = d;
	else if(doigt1_.id() == d.id())
		doigt1_ = d;
	else
		if(!doigt2_.existe())
			if(!doigt1_.existe())
				doigt1_ = d;
			else
				doigt2_ = d;
		else
			if(!doigt1_.existe())
				doigt1_ = d;
			// else
				// throw "ERREUR luigi::set_doigt";
}

void luigi::delete_doigt(int id) {
	if(doigt2_.id() == id)
		doigt2_ = doigt::aucun;
	else if(doigt1_.id() == id)
		doigt1_ = doigt::aucun;
	// else
		// throw "ERREUR luigi::delete_doigt";
}

// void luigi::set_doigt(int id_doigt, int x, int y) {
// 	if(doigt2_.id() == id_doigt) {
// 		doigt2_.x(x); doigt2_.y(y);
// 	} else if(doigt1_.id() == id_doigt) {
// 		doigt1_.x(x); doigt2_.y(y);
// 	} else
// 		throw "ERREUR luigi::set_doigt";
// }

doigt& luigi::premier_doigt_qui_passe() {
	if(!doigt1_.existe())
		return doigt2_;
	return doigt1_;
}

doigt luigi::point_median() {
	float x_med = (doigt1_.x() + doigt2_.x()) / 2;
	float y_med = (doigt2_.x() + doigt2_.y()) / 2;
	
	return doigt(x_med, y_med, doigt1_.id() + doigt2_.id());
}

float luigi::longueur() {
	return doigt::distance(doigt1_, doigt2_);
}


std::ostream & operator<<(std::ostream & out, luigi& l) {
	out << l.temps() << "::::" << l.doigt1() << "::::" << l.doigt2();
	return out;
}