#include <rvip.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <mario.h>
#include <peach.h>
#include <toad.h>
#include <algorithm>

OSG_USING_NAMESPACE

int signe(double n) {
	if(n > 0) return 1;
	if(n < 0) return -1;
	return 0;
}

struct inertie {
	static const double v_defaut = 0.01;
	
	inertie(double x = 0, double v = v_defaut) : x_(x), v_(v) {}
	void reset(double x = 0) { x_ = x; }
	void vitesse(double v = v_defaut) { v_ = v; }
	void move() {
		if(x_ < 0)
			x_ = std::min(x_ + v_, 0.0);
		else if(x_ > 0)
			x_ = std::max(0.0, x_ - v_);
	}
	double& operator()() { return x_; }
	bool fini() { return x_ == 0; }
private:
	double x_, v_;
};

struct peachtest : public peach, public toad {
	peachtest(gestionnaire_de_scene& g) : g_(g), doigt_original_orientation_(doigt::aucun) {}
	virtual ~peachtest() {}
	virtual void bip(double temps) {
		// Appelé à chaque frame
		dernier_temps_ = temps;
		mettre_a_jour();
		// Mise à jour de l'inertie
		maj_inertie();
		// Mise à jour des doigts
		g_.doigt1_set() = plombier_.situation().doigt1().existe();
		g_.doigt2_set() = plombier_.situation().doigt2().existe();
		g_.doigt1_x() = plombier_.situation().doigt1().x();
		g_.doigt1_y() = plombier_.situation().doigt1().y();
		g_.doigt2_x() = plombier_.situation().doigt2().x();
		g_.doigt2_y() = plombier_.situation().doigt2().y();
	}
	virtual double temps() { return dernier_temps_; }
	void maj_inertie() {
		if(!rotation_x_.fini() || !rotation_y_.fini()) {
			if(etat_actuel() != 2) {
				rotation_x_.move();
				rotation_y_.move();
			}
			if(!move_selection_)
				g_.camera_rotation(rotation_x_(), rotation_y_());
			else
				g_.selection_rotate(rotation_x_(), rotation_y_());
		}
		if(move_selection_ && (!mvt_x_.fini() || !mvt_y_.fini() || !mvt_z_.fini())) {
			if(etat_actuel() != 5) {
				mvt_x_.move();
				mvt_y_.move();
				mvt_z_.move();
			}
			g_.selection_move(mvt_x_(), mvt_y_(), mvt_z_());
		}
		if(!move_selection_ && !mvt_x_.fini()) {
			mvt_x_.move();
			g_.navigateur().right(mvt_x_());
		}
		if(!move_selection_ && !mvt_y_.fini()) {
			mvt_y_.move();
			g_.camera_move_y(mvt_y_());
		}
		if(!move_selection_ && !mvt_z_.fini()) {
			mvt_z_.move();
			g_.navigateur().forward(mvt_z_());
		}
	}
	virtual void mode_orientation(peach_t etat) {
		static double q = 0;
		if(etat == debut) {
			doigt_original_orientation_ = plombier_.situation().premier_doigt_qui_passe();
			q = 0;
			rotation_x_.reset(0);
			rotation_y_.reset(0);
		}
		else if(etat == en_cours) {
			if(!ok_move_) {
				move_selection_ = g_.est_sur_selection(doigt_original_orientation_.x(), doigt_original_orientation_.y());
				ok_move_ = true;
			}
			
			++q;
			
			if(q < 2)
				return;
			
			luigi& sit = plombier_.situation();
			doigt le_doigt = sit.premier_doigt_qui_passe();
			
			double ecart_x = le_doigt.x() - doigt_original_orientation_.x();
			double ecart_y = le_doigt.y() - doigt_original_orientation_.y();
			doigt_original_orientation_ = le_doigt;
			rotation_x_.reset(std::max(-0.3, std::min(ecart_x * M_PI, 0.3)));
			rotation_y_.reset(std::max(-0.14, std::min(ecart_y * M_PI, 0.14)));
		}
		else if(etat == fin) {
			if(etat_actuel_ != 5)
				ok_move_ = false;
			// if(q < 10) {
			// 	rotation_x_.reset(0);
			// 	rotation_y_.reset(0);
			// }
		}
	}
	virtual void mode_deplacement(peach_t etat) {
		if(etat == debut) {
			doigts_originaux_deplacement_ = plombier_.situation();
			mvt_x_.reset(0);
			mvt_y_.reset(0);
			mvt_z_.reset(0);
		}
		else if(etat == en_cours) {
			if(!ok_move_) {
				move_selection_ = g_.est_sur_selection(doigts_originaux_deplacement_.doigt1().x(), doigts_originaux_deplacement_.doigt1().y()) || g_.est_sur_selection(doigts_originaux_deplacement_.doigt2().x(), doigts_originaux_deplacement_.doigt2().y());
				ok_move_ = true;
			}
			
			luigi& sit = plombier_.situation();
			doigt dm_avant = doigts_originaux_deplacement_.point_median();
			doigt dm = sit.point_median();
			
			double longueur_avant = doigts_originaux_deplacement_.longueur();
			double longueur = sit.longueur();
			
			doigts_originaux_deplacement_ = plombier_.situation();
			double q = dm.vitesse() * 6.0;
			
			mvt_x_.reset(std::max(-0.3, std::min(0.3, (dm_avant.x() - dm.x())*q)));
			mvt_y_.reset(std::max(-0.3, std::min(0.3, (dm_avant.y() - dm.y())*q)));
			mvt_z_.reset(std::max(-0.3, std::min(0.3, (longueur_avant - longueur)*q)));
		}
		else if(etat == fin) {
			if(etat_actuel_ != 2)
				ok_move_ = false;
		}
	}
	virtual void selection() {
		doigt& le_doigt = debut_action_;
		// Il y a plusieurs trucs à cliquer :
		// - le bouton pour quitter
		if(le_doigt.x() <= 0.1 && le_doigt.y() >= 0.9) {
			g_.doit_quitter();
		} else if(g_.incarnation() != NullFC && le_doigt.x() >= 0.8 && le_doigt.y() >= 0.8) {
			g_.desincarner();
		} else
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
		static double aa = 0;
		if(etat == debut) {
			if(g_.selection() == NullFC || g_.incarnation() != NullFC) {
				etat_actuel_ = 2;
				mode_orientation(etat);
				return;
			}
			a = 0;
			g_.motion_blur(true);
			ancien_from_ = g_.navigateur().getFrom();
			ancien_at_ = g_.navigateur().getAt();
			ancienne_focale_ = g_.angle_focale();
			distance_incarnation_ = ancien_from_.dist(g_.hit_point_selection());
			nb_frames_a_remplir_ = 0.3 * C_ / g_.ms_par_frame();
			avancee_par_trame_ = distance_incarnation_ / nb_frames_a_remplir_;
			aa = avancee_par_trame_;
			dir_sel_ = g_.hit_point_selection() - g_.navigateur().getFrom();
			dir_sel_.normalize();
			dir_sel_ *= avancee_par_trame_;
			at_fin_ = g_.hit_point_selection() + dir_sel_;
			at_fin2_ = at_fin_;
		}
		else if(etat == en_cours) {
			if(g_.selection() == NullFC || g_.incarnation() != NullFC)
				return;
			++a;
			g_.angle_focale() += 0.6;
			
			if(g_.navigateur().getFrom().dist(g_.hit_point_selection()) <= avancee_par_trame_) {
				avancee_par_trame_ = 0;
			}
			
			if(g_.navigateur().getFrom().dist(g_.hit_point_selection()) <= distance_incarnation_ / 2)
				avancee_par_trame_ *= 0.98;
			
			
			// Avance vers la sélection
			at_courant_ = g_.navigateur().getAt();
			g_.navigateur().setAt(at_fin2_);
			g_.navigateur().forward(-1 * avancee_par_trame_);
			
			// Interpolation du at courant "pour faire style" de regarder
			double q = 0.92;
			
			at_courant_[0] = (q * at_courant_[0] + at_fin2_[0] * (1 - q));
			at_courant_[1] = (q * at_courant_[1] + at_fin2_[1] * (1 - q));
			at_courant_[2] = (q * at_courant_[2] + at_fin2_[2] * (1 - q));
			// On regarde par là en faisant style
			g_.navigateur().setAt(at_courant_);
		}
		else {
			if(g_.selection() == NullFC || g_.incarnation() != NullFC)
				return;
			
			g_.motion_blur(false);
			// Repositionne tout ça comme c'était avant
			at_courant_ = g_.navigateur().getFrom();
			g_.navigateur().setFrom(ancien_from_);
			g_.navigateur().setAt(ancien_at_);
			g_.angle_focale() = ancienne_focale_;
		}
	}
	virtual void incarnation() {
		if(g_.selection() == NullFC || g_.incarnation() != NullFC)
			return;
		g_.motion_blur(false);
		g_.navigateur().setFrom(at_fin_ - dir_sel_);
		g_.navigateur().setAt(at_fin_);
		g_.angle_focale() = ancienne_focale_;
		g_.incarner();
	}
private:
	int a;
	gestionnaire_de_scene& g_;
	inertie rotation_x_, rotation_y_, mvt_x_, mvt_y_, mvt_z_;
	OSG::Pnt3f ancien_from_, ancien_at_, at_fin_, at_fin2_, at_courant_, at_wayfinding_;
	OSG::Vec3f dir_sel_;
	double ancienne_focale_;
	double dernier_temps_;
	double distance_incarnation_;
	double nb_frames_a_remplir_, avancee_par_trame_;
	
	doigt doigt_original_orientation_;
	luigi doigts_originaux_deplacement_;
	bool wayfinding_en_cours_, move_selection_, ok_move_;
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