#if !defined(PEACH_H_SVS7187P)
#define PEACH_H_SVS7187P

#include "entrees.h"
#include "mario.h"

// en millisecondes
const double A_par_defaut = 100;
const double B_par_defaut = 700;
const double C_par_defaut = 2000;
const double seuil_distance_par_defaut = 0.05;

class graphe_incoherent {};

enum peach_t {
	debut, en_cours, fin
};

struct peach {
	peach(int port = 3333, double A = A_par_defaut, double B = B_par_defaut, double C = C_par_defaut, double seuil_dist = seuil_distance_par_defaut);
	// Mise à jour par l'arrivée d'un petit événement tactile : pas de mise à jour mais une avancée de l'automate
	void mettre_a_jour(evenement_touch_t);
	// Mise à jour par un top d'horloge : là, on fera des choses !
	void mettre_a_jour();
private:
	void avance_automate(evenement_touch_t);
	void avance_automate();
	void fais_des_choses();

protected:
	virtual void mode_orientation(peach_t);
	virtual void mode_deplacement(peach_t);
	virtual void selection();
	virtual void wayfinding();
	virtual void debut_incarnation(peach_t);
	virtual void incarnation();
	virtual double temps() { return std::clock() * 1000.0 / CLOCKS_PER_SEC; }
private:
	TUIO::TuioClient client_;
protected:
	mario plombier_;
private:
	enum { taille_historique_ = 3 };
protected:
	int etat_actuel_;
	double A_, B_, C_;
private:
	double seuil_distance_;
	double tempo_actuelle_;
	doigt davant_;
protected:
	doigt debut_action_;
};

#endif /* end of include guard: PEACH_H_SVS7187P */
