#include <rvip.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <mario.h>
#include <peach.h>
#include <toad.h>

OSG_USING_NAMESPACE

int signe(double n) {
	if(n > 0) return 1;
	if(n < 0) return -1;
	return 0;
}

struct peachtest : public peach, public toad {
	peachtest(gestionnaire_de_scene& g) : g_(g), doigt_original_orientation_(doigt::aucun) {}
	virtual ~peachtest() {}
	virtual void bip(double temps) {
		dernier_temps_ = temps;
		mettre_a_jour();
	}
	virtual double temps() { return dernier_temps_; }
	virtual void mode_orientation(peach_t etat) {
		if(etat == debut) {
			doigt_original_orientation_ = plombier_.situation().premier_doigt_qui_passe();
		}
		else if(etat == en_cours) {
			move_selection_ = g_.est_sur_selection(doigt_original_orientation_.x(), doigt_original_orientation_.y());
			luigi& sit = plombier_.situation();
			doigt le_doigt = sit.premier_doigt_qui_passe();
			
			double ecart_x = le_doigt.x() - doigt_original_orientation_.x();
			double ecart_y = le_doigt.y() - doigt_original_orientation_.y();
			
			if(!move_selection_)
				g_.navigateur().rotate(ecart_x *0.5, ecart_y *0.5);
			else
				;
		}
		else if(etat == fin) {
			move_selection_ = false;
		}
	}
	virtual void mode_deplacement(peach_t etat) {
		/*OSG::Node selec = selection();
		if(selec)	
			g_.pointprojete(debut_action_.x(), debut_action_.y(),);*/
		
		if(etat == debut) {
			doigts_originaux_deplacement_ = plombier_.situation();
		}
		else if(etat == en_cours) {
			luigi& sit = plombier_.situation();
			doigt dm_avant = doigts_originaux_deplacement_.point_median();
			doigt dm = sit.point_median();
			
			double longueur_avant = doigts_originaux_deplacement_.longueur();
			double longueur = sit.longueur();
			
			g_.navigateur().right(dm_avant.x() - dm.x());
			g_.navigateur().forward(longueur_avant - longueur);
			
			OSG::Pnt3f from = g_.navigateur().getFrom();
			from[1] += dm_avant.y() - dm.y();
			g_.navigateur().setFrom(from);
			// Commenter les trois lignes de la suite pour un effet sympa	
			OSG::Pnt3f at = g_.navigateur().getAt();
			at[1] += dm_avant.y() - dm.y();
			g_.navigateur().setAt(at);
		}
		else if(etat == fin) {
			
		}
	}
	virtual void selection() {
		doigt& le_doigt = debut_action_;
		g_.selectionner(le_doigt.x(), le_doigt.y());
	}
	virtual void wayfinding() {
		if(!wayfinding_en_cours_) {
			ancien_from_ = g_.navigateur().getFrom();
			ancien_at_ = g_.navigateur().getAt();
			vitesse_wayfinding_ = 0;
			if(g_.point_projete(debut_action_.x(), debut_action_.y(), at_wayfinding_)) {
				accel_wayfinding_ = g_.navigateur().getFrom().dist(at_wayfinding_) * 0.001;
				wayfinding_en_cours_ = true;
			}
			else
				etat_actuel_ = 0;
		}
		if(wayfinding_en_cours_) {
			// Avance vers la sélection
			at_courant_ = g_.navigateur().getAt();
			g_.navigateur().setAt(at_wayfinding_);
			g_.navigateur().forward(-1 * vitesse_wayfinding_);
			vitesse_wayfinding_ += std::max(0.0, accel_wayfinding_);
			
			// Interpolation du at courant "pour faire style" de regarder
			static const double q = 0.9;
			at_courant_[0] = (q * at_courant_[0] + at_wayfinding_[0] * (1 - q));
			at_courant_[1] = (q * at_courant_[1] + at_wayfinding_[1] * (1 - q));
			at_courant_[2] = (q * at_courant_[2] + at_wayfinding_[2] * (1 - q));
			// On regarde par là en faisant style
			g_.navigateur().setAt(at_courant_);
			// Plus on s'approche, plus on ralentit à partir d'un certain temps
			if(g_.navigateur().getFrom().dist(at_wayfinding_) < 10 * vitesse_wayfinding_)
				accel_wayfinding_ -= g_.navigateur().getFrom().dist(at_wayfinding_) * 0.001;
			// Condition de sortie : on est arrivés devant
			if(g_.navigateur().getFrom().dist(at_wayfinding_) < 3 * vitesse_wayfinding_) {
				wayfinding_en_cours_ = false;
				etat_actuel_ = 0;
			}
		}
	}
	virtual void debut_incarnation(peach_t etat) {
		if(etat == debut) {
			g_.motion_blur(true);
			ancien_from_ = g_.navigateur().getFrom();
			ancien_at_ = g_.navigateur().getAt();
			ancienne_focale_ = g_.angle_focale();
			distance_incarnation_ = ancien_from_.dist(g_.hit_point_selection());
			nb_frames_a_remplir_ = 1.0 * C_ / g_.ms_par_frame();
			avancee_par_trame_ = distance_incarnation_ / nb_frames_a_remplir_ * 3;
			milieu_[0] = (ancien_from_[0] + g_.hit_point_selection()[0]) / 2;
			milieu_[1] = (ancien_from_[1] + g_.hit_point_selection()[1]) / 2;
			milieu_[2] = (ancien_from_[2] + g_.hit_point_selection()[2]) / 2;
		}
		else if(etat == en_cours) {
			g_.angle_focale() += 0.6;
			
			// Avance vers la sélection
			at_courant_ = g_.navigateur().getAt();
			g_.navigateur().setAt(g_.hit_point_selection());
			g_.navigateur().forward(-1 * avancee_par_trame_);
			// TODO: mal fait
			// Interpolation du at courant "pour faire style" de regarder
			// double q = 0.92;
			
			// at_courant_[0] = (q * at_courant_[0] + g_.hit_point_selection()[0] * (1 - q));
			// at_courant_[1] = (q * at_courant_[1] + g_.hit_point_selection()[1] * (1 - q));
			// at_courant_[2] = (q * at_courant_[2] + g_.hit_point_selection()[2] * (1 - q));
			
			// On regarde par là en faisant style
			// g_.navigateur().setAt(at_courant_);
			g_.navigateur().setAt(milieu_);
		}
		else {
			g_.motion_blur(false);
			at_courant_ = g_.navigateur().getFrom();
			// Repositionne tout ça comme c'était avant
			g_.navigateur().setFrom(ancien_from_);
			ancien_from_ = g_.navigateur().getAt();
			g_.navigateur().setAt(ancien_at_);
			g_.angle_focale() = ancienne_focale_;
		}
	}
	virtual void incarnation() {
		g_.motion_blur(false);
		g_.navigateur().setFrom(at_courant_);
		g_.navigateur().setAt(ancien_from_);
		g_.angle_focale() = ancienne_focale_;
	}
private:
	gestionnaire_de_scene& g_;
	OSG::Pnt3f ancien_from_, ancien_at_, at_courant_, at_wayfinding_, milieu_;
	double ancienne_focale_;
	double dernier_temps_;
	double distance_incarnation_;
	double nb_frames_a_remplir_, avancee_par_trame_;
	
	doigt doigt_original_orientation_;
	luigi doigts_originaux_deplacement_;
	bool wayfinding_en_cours_, move_selection_;
	double vitesse_wayfinding_, accel_wayfinding_;
};

int main(int argc, char **argv) {
	gestionnaire_de_scene g(&argc, argv); 
#ifdef WIN32
	g.attacher(SceneFileHandler::the().read("data\\contest.wrl"));
#else
	g.attacher(SceneFileHandler::the().read("data/contest.wrl"));
#endif
	peachtest tuiote_moi_ca(g);
	g.ajouter_toad(tuiote_moi_ca);
	g.executer();	
	return 0;
}