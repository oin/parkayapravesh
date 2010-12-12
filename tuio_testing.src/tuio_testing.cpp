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
		/*
			TODO Il faut gérer les rotations comme ça : dans gestionnaire_de_scene, on devrait avoir uniquement rx, ry, rz (pitch, yaw, roll), et la méthode animation devrait convertir ça en quaternion valable (voir http://gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_represent_rotation et http://www.opensg.org/htdocs/doc-1.8/classosg_1_1Quaternion.html et /Users/join/src/opensg/OpenSG/Source/System/Window/OSGTrackball.cpp)
		*/
	if(etat == debut) {
				doigt_original_orientation_ = plombier_.situation().premier_doigt_qui_passe();
			}
			else if(etat == en_cours) {
				luigi& sit = plombier_.situation();
				doigt le_doigt = sit.premier_doigt_qui_passe();
				
				double ecart_x = le_doigt.x() - doigt_original_orientation_.x();
				double ecart_y = le_doigt.y() - doigt_original_orientation_.y();
				int sens_x = 0, sens_y = 0;
				
				double epsilon_x = 0.01;
				
				double epsilon_y = 0.01;
				
				g_.rx() = signe(ecart_x);
				g_.ry() = signe(ecart_y);
				double longueur = doigt::distance(doigt_original_orientation_, le_doigt);
				g_.rtheta() += longueur;
			}
			else if(etat == fin) {
				
			}
	}
	virtual void mode_deplacement(peach_t etat) {
		if(etat == debut) {
			doigts_originaux_deplacement_ = plombier_.situation();
			// std::cout << "Situation originale : " << doigts_originaux_deplacement_ << std::endl;
		}
		else if(etat == en_cours) {
			luigi& sit = plombier_.situation();
			doigt dm_avant = doigts_originaux_deplacement_.point_median();
			doigt dm = sit.point_median();
			
			double longueur_avant = doigts_originaux_deplacement_.longueur();
			double longueur = sit.longueur();
			
			g_.x() += (dm.x() - dm_avant.x());
			g_.y() -= (dm.y() - dm_avant.y());
			g_.z() -= longueur - longueur_avant;
		}
		else if(etat == fin) {
			
		}
	}
	virtual void selection() {
		
	}
	virtual void wayfinding() {
		
	}
	virtual void debut_incarnation(peach_t etat) {
		if(etat == debut) {
			g_.motion_blur(true);
			ancienne_avancee_ = g_.z();
			ancienne_focale_ = g_.angle_focale();
		}
		else if(etat == en_cours) {
			g_.angle_focale() += 0.6;
			g_.z() -= 0.1;
		}
		else {
			g_.motion_blur(false);
			g_.z() = ancienne_avancee_;
			g_.angle_focale() = ancienne_focale_;
		}
	}
	virtual void incarnation() {
		g_.motion_blur(false);
		g_.z() = ancienne_avancee_;
		g_.angle_focale() = ancienne_focale_;
	}
private:
	gestionnaire_de_scene& g_;
	double ancienne_avancee_, ancienne_focale_;
	double dernier_temps_;
	
	doigt doigt_original_orientation_;
	luigi doigts_originaux_deplacement_;
};

int main(int argc, char **argv) {
	gestionnaire_de_scene g(&argc, argv); 
	g.attacher(SceneFileHandler::the().read("data/contest.wrl"));
	peachtest tuiote_moi_ca(g);
	g.ajouter_toad(tuiote_moi_ca);
	g.executer();	
	return 0;
}