#if !defined(GESTIONNAIRE_DE_SCENE_H_SHWVIRI4)
#define GESTIONNAIRE_DE_SCENE_H_SHWVIRI4

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGGradientBackground.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGBaseFunctions.h>
#include <OpenSG/OSGTransform.h>

#include "coords.h"

struct gestionnaire_de_scene {
	gestionnaire_de_scene(int*, char**);
	virtual ~gestionnaire_de_scene();
	static gestionnaire_de_scene& instance() { return *instance_; }
	void executer() const;
	void attacher(OSG::NodePtr);
	virtual void doit_quitter();
protected:
	gestionnaire_de_scene() {}
	virtual void init(int*, char**);
	virtual OSG::NodePtr init_root();
	virtual OSG::NodePtr creer_noeud_de_depart();
	static void redimensionnement_fenetre(int,int);
	static void affichage_fenetre(void);
	static void mouvement_souris(int,int);
	static void bouton_souris(int,int,int,int);
	static void clavier(unsigned char, int, int);
	static void idle(void);
	virtual std::string titre() { return "Scene pas terrible"; }
	virtual void animation() {}
	virtual bool souris(int bouton, int etat, int x, int y) { return true; }
	virtual bool mvt_souris(int x, int y) { return true; }
	virtual bool frappe_clavier(unsigned char key, int x, int y) { return true; }
private:
	int setupGLUT(int* argc, char** argv);
	
	OSG::NodePtr noeud_root_;
	OSG::NodePtr cam_beacon_;
	OSG::NodePtr light_beacon_;
	OSG::NodePtr noeud_depart_;
	OSG::TransformPtr cam_transform_;
	OSG::TransformPtr light_transform_;
	OSG::WindowPtr fenetre_;
	OSG::ViewportPtr viewport_;
	OSG::RenderAction* render_action_;
	OSG::PerspectiveCameraPtr camera_;
	coords coords_initiales_;
	coords coords_;
	
	static gestionnaire_de_scene* instance_;
};

#endif /* end of include guard: GESTIONNAIRE_DE_SCENE_H_SHWVIRI4 */
