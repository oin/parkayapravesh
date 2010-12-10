#if !defined(PEACH_H_SVS7187P)
#define PEACH_H_SVS7187P

#include "entrees.h"
#include "mario.h"

const double A_par_defaut = 0.1 * CLOCKS_PER_SEC;
const double B_par_defaut = CLOCKS_PER_SEC;
const double C_par_defaut = CLOCKS_PER_SEC;
const double seuil_distance_par_defaut = 0.001;

class graphe_incoherent {};

struct peach {
	peach(int port = 3333, double A = A_par_defaut, double B = B_par_defaut, double C = C_par_defaut, double seuil_dist = seuil_distance_par_defaut);
	// Mise à jour par l'arrivée d'un petit paquet
	void mettre_a_jour(evenement_touch_t);
	void mettre_a_jour();
private:
	void avance_automate(evenement_touch_t);
	void avance_automate();
	void fais_des_choses();
	
	void mode_orientation();
	void mode_deplacement();
	void selection();
	void wayfinding();
	void debut_incarnation();
	void incarnation();
	
	TUIO::TuioClient client_;
	mario plombier_;
	enum { taille_historique_ = 3 };
	int etat_actuel_;
	double A_, B_, C_;
	double seuil_distance_;
	double tempo_actuelle_;
};

#endif /* end of include guard: PEACH_H_SVS7187P */
