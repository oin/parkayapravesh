#include "peach.h"

peach::peach(int port, double A, double B, double C, double seuil_distance) : client_(port), plombier_(this), etat_actuel_(0), A_(A), B_(B), C_(C), seuil_distance_(seuil_distance), tempo_actuelle_(-1) {
	client_.addTuioListener(&plombier_);
	client_.connect();
}

void peach::mettre_a_jour(evenement_touch_t e) {
	static doigt davant = doigt::aucun;
	doigt& premierkipass = plombier_.situation().premier_doigt_qui_passe();
	// Essaie d'ignorer quelques événements dans certains cas
	if(e == e_update && (etat_actuel_ == 1 || etat_actuel_ == 3 || etat_actuel_ == 6)) {
		double distance_parcourue = doigt::distance(premierkipass, davant);
		if(distance_parcourue < seuil_distance_)
			return;
	}
	davant = premierkipass;
	
	avance_automate(e);
	fais_des_choses();
}

void peach::mettre_a_jour() {
	avance_automate();
	fais_des_choses();
	if(tempo_actuelle_ != -1)
	std::cout << std::clock() - tempo_actuelle_ << std::endl;
}

void peach::fais_des_choses() {
	switch(etat_actuel_) {
		case 2: // mode orientation
			mode_orientation();
			break;
		case 3: // doigt -> pas doigt
			break;
		case 4: // début de l'incarnation
			debut_incarnation();
			break;
		case 5: // déplacement
			mode_deplacement();
			break;
		// case 6: // doigt -> pas doigt -> doigt
		// 	break;
		case 7: // wayfinding
			wayfinding();
			etat_actuel_ = 0;
			std::cout << "0" << std::endl;
			break;
		case 8: // sélection
			selection();
			etat_actuel_ = 0;
			std::cout << "0" << std::endl;
			break;
		case 9: // incarnation
			incarnation();
			etat_actuel_ = 0;
			std::cout << "0" << std::endl;
			break;
	}
}

void peach::avance_automate() {
	// On évalue le résultat de temporisations
	
	// Si aucune tempo n'est en attente, on s'en va
	if(tempo_actuelle_ == -1) return;
	
	// Sinon, on en mate quelques-unes
	if(etat_actuel_ == 1 && std::clock() - tempo_actuelle_ > B_) {
		etat_actuel_ = 4;
		tempo_actuelle_ = std::clock();
	}
	else if(etat_actuel_ == 4 && std::clock() - tempo_actuelle_ > C_)
		etat_actuel_ = 9;
	else if(etat_actuel_ == 4 && std::clock() - tempo_actuelle_ < C_) {
		std::cout << std::clock() - tempo_actuelle_ << " < " << C_ << std::endl;
	}
	else if(etat_actuel_ == 3 && std::clock() - tempo_actuelle_ > A_)
		etat_actuel_ = 8;
	else
		return;
	
	// Si une tempo s'est terminée :
	tempo_actuelle_ = -1;
}

void peach::avance_automate(evenement_touch_t e) {
	// On fait avancer l'automate
	switch(etat_actuel_) {
		case 0: // état initial
			if(e == e_add) {
				etat_actuel_ = 1;
				tempo_actuelle_ = std::clock();
			}
			break;
		case 1: // doigt
			if(e == e_add) {
				etat_actuel_ = 5;
				tempo_actuelle_ = -1;
			}
			else if(e == e_del) {
				etat_actuel_ = 3;
				tempo_actuelle_ = std::clock();
			}
			break;
		case 2: // mode orientation
			if(e == e_add) {
				etat_actuel_ = 5;
				tempo_actuelle_ = -1;
			}
			else if(e == e_del) {
				etat_actuel_ = 0;
				tempo_actuelle_ = -1;
				std::cout << "0" << std::endl;
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
				std::cout << "0" << std::endl;
			}
			break;
		case 5: // déplacement
			if(e == e_del) {
				etat_actuel_ = 2;
				tempo_actuelle_ = -1;
			}
			break;
		case 6: // doigt -> pas doigt -> doigt
			if(e == e_add) {
				etat_actuel_ = 5;
				tempo_actuelle_ = -1;
			}
			else if(e == e_del) {
				etat_actuel_ = 7;
				tempo_actuelle_ = -1;
			}
			else {//if(e == e_update)
				etat_actuel_ = 2;
				tempo_actuelle_ = -1;
			}
			break;
		case 7: // wayfinding
		case 8: // sélection
		case 9: // incarnation
		case 99:// reset
			etat_actuel_ = 0;
	}
}

void peach::mode_orientation() {
	std::cout << "Mode orientation" << std::endl;
}

void peach::mode_deplacement() {
	std::cout << "Mode déplacement" << std::endl;
}

void peach::selection() {
	std::cout << "Sélection" << std::endl;
}

void peach::wayfinding() {
	std::cout << "Wayfinding" << std::endl;
}

void peach::debut_incarnation() {
	std::cout << "Début incarnation" << std::endl;
}

void peach::incarnation() {
	std::cout << "Incarnation" << std::endl;
}
