#include "peach.h"

peach::peach(int port, double A, double B, double C, double seuil_distance) : client_(port), plombier_(this), etat_actuel_(0), A_(A), B_(B), C_(C), seuil_distance_(seuil_distance), tempo_actuelle_(-1), davant_(doigt::aucun), debut_action_(doigt::aucun) {
	client_.addTuioListener(&plombier_);
	client_.connect();
}

void peach::mettre_a_jour(evenement_touch_t e) {
	doigt& premierkipass = plombier_.situation().premier_doigt_qui_passe();
	// Essaie d'ignorer quelques événements dans certains cas
	if(e == e_update && (etat_actuel_ == 1 || etat_actuel_ == 3 || etat_actuel_ == 6)) {
		double distance_parcourue = doigt::distance(premierkipass, davant_);
		if(distance_parcourue < seuil_distance_)
			return;
	}
	davant_ = premierkipass;
	
	avance_automate(e);
	// fais_des_choses();
}

void peach::mettre_a_jour() {
	avance_automate();
	fais_des_choses();
}

void peach::fais_des_choses() {
	switch(etat_actuel_) {
		case 2: // mode orientation
			mode_orientation(en_cours);
			break;
		case 3: // doigt -> pas doigt
			break;
		case 4: // début de l'incarnation
			debut_incarnation(en_cours);
			break;
		case 5: // déplacement
			mode_deplacement(en_cours);
			break;
		case 7: // wayfinding
			wayfinding();
			break;
		case 8: // sélection
			selection();
			etat_actuel_ = 0;
			break;
		case 9: // incarnation
			incarnation();
			etat_actuel_ = 0;
			break;
	}
}

void peach::avance_automate() {
	// On évalue le résultat de temporisations
	
	// Si aucune tempo n'est en attente, on s'en va
	if(tempo_actuelle_ == -1) return;
	
	// Sinon, on en mate quelques-unes
	if(etat_actuel_ == 1 && temps() - tempo_actuelle_ > B_) {
		etat_actuel_ = 4;
		tempo_actuelle_ = temps();
		debut_incarnation(debut);
		return;
	}
	else if(etat_actuel_ == 4 && temps() - tempo_actuelle_ > C_)
		etat_actuel_ = 9;
	else if(etat_actuel_ == 3 && temps() - tempo_actuelle_ > A_)
		etat_actuel_ = 8;
	else
		return;
	
	// Si une tempo s'est terminée :
	tempo_actuelle_ = -1;
}

void peach::avance_automate(evenement_touch_t e) {
	int etat_avant_ = etat_actuel_;
	
	// On fait avancer l'automate
	switch(etat_actuel_) {
		case 0: // état initial
			if(e == e_add) {
				etat_actuel_ = 1;
				tempo_actuelle_ = temps();
				debut_action_ = plombier_.situation().premier_doigt_qui_passe();
			}
			break;
		case 1: // doigt
			if(e == e_add) {
				etat_actuel_ = 5;
				tempo_actuelle_ = -1;
				mode_deplacement(debut);
			}
			else if(e == e_del) {
				etat_actuel_ = 3;
				tempo_actuelle_ = temps();
			}
			else if(e == e_update) {
				etat_actuel_ = 2;
				tempo_actuelle_ = -1;
				mode_orientation(debut);
			}
			break;
		case 2: // mode orientation
			if(e == e_add) {
				etat_actuel_ = 5;
				tempo_actuelle_ = -1;
				mode_orientation(fin);
				mode_deplacement(debut);
			}
			else if(e == e_del) {
				etat_actuel_ = 0;
				tempo_actuelle_ = -1;
				mode_orientation(fin);
			}
			break;
		case 3: // doigt -> pas doigt
			if(e == e_add) {
				etat_actuel_ = 6;
				tempo_actuelle_ = -1;
			}
			break;
		case 4: // début de l'incarnation
			if(e == e_del) {
				etat_actuel_ = 0;
				tempo_actuelle_ = -1;
				debut_incarnation(fin);
			}
			break;
		case 5: // déplacement
			if(e == e_del) {
				etat_actuel_ = 2;
				tempo_actuelle_ = -1;
				mode_deplacement(fin);
				mode_orientation(debut);
			}
			break;
		case 6: // doigt -> pas doigt -> doigt
			if(e == e_add) {
				etat_actuel_ = 5;
				tempo_actuelle_ = -1;
				mode_deplacement(debut);
			}
			else if(e == e_del) {
				etat_actuel_ = 7;
				tempo_actuelle_ = -1;
			}
			else {//if(e == e_update)
				etat_actuel_ = 2;
				tempo_actuelle_ = -1;
				mode_orientation(debut);
			}
			break;
		case 8: // sélection
		case 9: // incarnation
		case 99:// reset
			etat_actuel_ = 0;
	}
	
	// if(etat_actuel_ != etat_avant_)
		// std::cout << "Etat " << etat_actuel_ << std::endl;
}

void peach::mode_orientation(peach_t etat) {
	std::cout << "Mode orientation";
	if(etat == debut)
		std::cout << " (début)";
	else if(etat == fin)
		std::cout << " (fin)";
	std::cout << std::endl;
}

void peach::mode_deplacement(peach_t etat) {
	std::cout << "Mode déplacement";
	if(etat == debut)
		std::cout << " (début)";
	else if(etat == fin)
		std::cout << " (fin)";
	std::cout << std::endl;
}

void peach::selection() {
	std::cout << "Sélection";
}

void peach::wayfinding() {
	std::cout << "Wayfinding" << std::endl;
	// Fin
	etat_actuel_ = 0;
}

void peach::debut_incarnation(peach_t etat) {
	std::cout << "Début incarnation";
	if(etat == debut)
		std::cout << " (début)";
	else if(etat == fin)
		std::cout << " (fin)";
	std::cout << std::endl;
}

void peach::incarnation() {
	std::cout << "Incarnation" << std::endl;
}
