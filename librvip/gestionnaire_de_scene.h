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
#include <vector>
#include "toad.h"

struct gestionnaire_de_scene {
	gestionnaire_de_scene(int*, char**, std::string titre = "Scene pas terrible");
	virtual ~gestionnaire_de_scene();
	static gestionnaire_de_scene& instance() { return *instance_; }
	void executer() const;
	void attacher(OSG::NodePtr);
	virtual void doit_quitter();
	void ajouter_toad(toad&);
	int ms_par_frame() { return ms_par_frame_; }
	
	// double& x() { return x_; }
	// 	double& y() { return y_; }
	// 	double& z() { return z_; }
	// 	double& rx() { return rx_; }
	// 	double& ry() { return ry_; }
	// 	double& rz() { return rz_; }
	double& angle_focale() { return angle_focale_; }
	
	void selectionner(double x, double y);
	OSG::NodePtr selection() { return selection_; }
	OSG::Pnt3f hit_point_selection() { return dernier_hit_point_selection_; }
	
	OSG::FlyNavigator& navigateur() { return navigateur_; }
	
	bool motion_blur() const { return motion_blur_; }
	void motion_blur(bool m);
	void toggle_motion_blur() { motion_blur(!motion_blur_); }
	bool point_projete(double x, double y, OSG::Pnt3f& pnt);
	OSG::PerspectiveCameraPtr camera() { return camera_; }
	void PersonnViewOff();
	void PersonnViewOn();
protected:
	gestionnaire_de_scene() {}
	virtual void init(int*, char**);
	virtual OSG::NodePtr init_root();
	
	static void redimensionnement_fenetre(int,int);
	static void affichage_fenetre(void);
	static void mouvement_souris(int,int);
	static void bouton_souris(int,int,int,int);
	static void clavier(unsigned char, int, int);
	static void idle(void);
	static void timouze(int);
	virtual std::string titre() { return titre_; }
	virtual void animation();
	virtual void placer_camera();
	virtual bool souris(int bouton, int etat, int x, int y) { return true; }
	virtual bool mvt_souris(int x, int y) { return true; }
	virtual bool frappe_clavier(unsigned char key, int x, int y) { return true; }
	
	void iconeFermer();
	void Cadre();

private:
	int setupGLUT(int* argc, char** argv);
	
	OSG::NodePtr noeud_root_;
	OSG::NodePtr cam_beacon_, cam_beacon2_, cam_beacon3_, cam2_beacon_;
	OSG::NodePtr light_beacon_;
	OSG::NodePtr noeud_depart_;
	OSG::TransformPtr cam_transform_,cam2_transform_, cam_transform2_, cam_transform3_;
	OSG::TransformPtr light_transform_;
	OSG::WindowPtr fenetre_;
	OSG::ViewportPtr viewport_;
	OSG::ViewportPtr viewport2_;
	OSG::RenderAction* render_action_;
	OSG::PerspectiveCameraPtr camera_;
	OSG::PerspectiveCameraPtr camera2_;
	OSG::NodePtr selection_, incarnation_;
	
	std::string titre_;
	
	double x_, y_, z_;
	double rx_, ry_, rz_;
	double angle_focale_;
	
	OSG::FlyNavigator navigateur_;
	
	double fps_;
	int ms_par_frame_;
	
	// En millisecondes
	double temps_depart_, temps_precedent_, temps_courant_, temps_depuis_trame_precedente_;
	
	bool motion_blur_, faire_clear_accum_;
	
	std::vector<toad*> toads_a_biper_;
	
	static gestionnaire_de_scene* instance_;
	
	// Gestion du highlight (pompé sur OSGSimpleSceneManager)
	OSG::NodeRefPtr                   _highlight;
    OSG::NodePtr                      _highlightNode;
    OSG::GeoPositions3fPtr            _highlightPoints;
	OSG::SimpleMaterialPtr    _highlightMaterial;
	
	OSG::Pnt3f dernier_hit_point_selection_;

protected:
	virtual void  highlightChanged(void);
    virtual void  updateHighlight (void);
};

#endif /* end of include guard: GESTIONNAIRE_DE_SCENE_H_SHWVIRI4 */
