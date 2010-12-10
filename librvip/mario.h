#if !defined(MARIO_H_EX5LSANN)
#define MARIO_H_EX5LSANN

#include "entrees.h"
#include <TuioListener.h>
#include <TuioClient.h>

class peach;

// Mario gère les tuyaux, mon gros !
// On n'utilise pas les messages REFRESH de TUIO pour gérer le temps, on utilisera les LUIGI !
struct mario : public TUIO::TuioListener {
	mario(peach* p) : a_notifier_(p) {}
	luigi& situation() { return situation_actuelle_; }
	
	void addTuioObject(TUIO::TuioObject *tobj) {}
	void updateTuioObject(TUIO::TuioObject *tobj) {}
	void removeTuioObject(TUIO::TuioObject *tobj) {}

	void addTuioCursor(TUIO::TuioCursor *tcur);
	void updateTuioCursor(TUIO::TuioCursor *tcur);
	void removeTuioCursor(TUIO::TuioCursor *tcur);

	void refresh(TUIO::TuioTime frameTime) {}
private:
	luigi situation_actuelle_;
	peach* a_notifier_;
};

#endif /* end of include guard: MARIO_H_EX5LSANN */
