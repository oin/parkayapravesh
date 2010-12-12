#include "mario.h"
#include "peach.h"
#include <iostream>

void mario::addTuioCursor(TUIO::TuioCursor *tcur) {
	situation_actuelle_.set_doigt(doigt(tcur->getX(), tcur->getY(), tcur->getCursorID()));
	situation_actuelle_.set_temps();
	a_notifier_->mettre_a_jour(e_add);
}

void mario::updateTuioCursor(TUIO::TuioCursor *tcur) {
	situation_actuelle_.set_doigt(doigt(tcur->getX(), tcur->getY(), tcur->getCursorID(), tcur->getMotionSpeed(), tcur->getMotionAccel()));
	situation_actuelle_.set_temps();
	a_notifier_->mettre_a_jour(e_update);
}

void mario::removeTuioCursor(TUIO::TuioCursor *tcur) {
	situation_actuelle_.delete_doigt(tcur->getCursorID());
	situation_actuelle_.set_temps();
	a_notifier_->mettre_a_jour(e_del);
}