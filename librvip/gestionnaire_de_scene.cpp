#include "gestionnaire_de_scene.h"
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGPointLight.h>
#include <OpenSG/OSGPassiveViewport.h>
#include <cstdlib>
#include <ctime>

OSG_USING_NAMESPACE;

gestionnaire_de_scene* gestionnaire_de_scene::instance_ = 0;

gestionnaire_de_scene::gestionnaire_de_scene(int* argc, char** argv, std::string titre) : titre_(titre), x_(0), y_(3.5), z_(10), rx_(0), ry_(0), rz_(0), rtheta_(0), angle_focale_(90), fps_(50), ms_par_frame_(static_cast<int>(1.0/fps_ * 1000)), motion_blur_(false) {
	init(argc, argv);
}

void gestionnaire_de_scene::ajouter_toad(toad& t) {
	toads_a_biper_.push_back(&t);
}

void gestionnaire_de_scene::init(int* argc, char** argv) {
	if(instance_ != 0)
		throw "Une seule scène peut être lancée.";
	instance_ = this;
	
	// Initialisation de GLUT
	int id_fenetre = setupGLUT(argc, argv);
	
	// Initialisation du sous-système OpenSG
	osgInit(*argc, argv);
	
	// Création d'une fenêtre GLUT
	GLUTWindowPtr fenetre = GLUTWindow::create();
	fenetre->setId(id_fenetre);
	fenetre->setSize(640, 480);
	fenetre->init();
	fenetre_ = fenetre;
	
	// Création d'une camera
	camera_ = PerspectiveCamera::create();
	
	// Création du nœud root
	noeud_root_ = init_root();
	
	// Mise en place d'un arrière-plan en dégradé
	GradientBackgroundPtr bkg = GradientBackground::create();
	beginEditCP(bkg);
	{
		bkg->addLine(Color3f(0.5,0.5,0.5), 0);
		bkg->addLine(Color3f(0.8,0.8,0.8), 1);
	}
	endEditCP(bkg);
	
	// Mise en place d'un viewport
	viewport_ = Viewport::create();
	beginEditCP(viewport_);
	{
		viewport_->setCamera(camera_);
		viewport_->setRoot(noeud_root_);
		viewport_->setBackground(bkg);
		viewport_->setSize(0, 0, 1, 1);
	}
	endEditCP(viewport_);
	
	// Ajoute le viewport à la fenêtre
	fenetre_->addPort(viewport_);
	
	// Crée une action de rendu
	render_action_ = RenderAction::create();
	render_action_->setWindow(fenetre_.getCPtr());
}

void gestionnaire_de_scene::doit_quitter() {
	OSG::osgExit();
	exit(0);
}

gestionnaire_de_scene::~gestionnaire_de_scene() {
	instance_ = 0;
}

void gestionnaire_de_scene::motion_blur(bool m) { 
	motion_blur_ = m;
	if(!m)
		faire_clear_accum_ = true;
}

void gestionnaire_de_scene::animation() {
	// Focale de la camera
	beginEditCP(camera_);
		camera_->setFov(deg2rad(angle_focale_));
	endEditCP(camera_);
	// Mouvement de la camera
	beginEditCP(cam_beacon_);
	beginEditCP(cam_transform_);
	{
		Matrix M;
		M.setTranslate(Vec3f(x_,y_,z_));
		M.setRotate(Quaternion(Vec3f(rx_,ry_,rz_), rtheta_));
		cam_transform_->setMatrix(M);
	}
	endEditCP(instance_->cam_transform_);
	endEditCP(instance_->cam_beacon_);
}

int gestionnaire_de_scene::setupGLUT(int* argc, char **argv) {
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE | GLUT_ACCUM);
	
	
	int id_fenetre = glutCreateWindow(titre().c_str());
	
	glutDisplayFunc(gestionnaire_de_scene::affichage_fenetre);
	// glutIdleFunc(gestionnaire_de_scene::idle);
	glutReshapeFunc(gestionnaire_de_scene::redimensionnement_fenetre);
	glutMotionFunc(gestionnaire_de_scene::mouvement_souris);
	glutMouseFunc(gestionnaire_de_scene::bouton_souris);
	glutKeyboardFunc(gestionnaire_de_scene::clavier);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glutFullScreen();
	glutTimerFunc(ms_par_frame_ / 2, gestionnaire_de_scene::timouze, 0);
	
	temps_depart_ = glutGet(GLUT_ELAPSED_TIME);
	temps_precedent_ = temps_depart_;
	
	return id_fenetre;
}

void gestionnaire_de_scene::clavier(unsigned char key, int x, int y) {
	if(instance_->frappe_clavier(key,x,y)) {
		// Action par défaut
		if(key == 27) { // ECHAP
			instance_->doit_quitter();
		} else if(key == 'b')
			instance_->toggle_motion_blur();
	}
}

void gestionnaire_de_scene::redimensionnement_fenetre(int w, int h) {
	instance_->fenetre_->resize(w, h);
	glutPostRedisplay();
}

void gestionnaire_de_scene::affichage_fenetre(void) {
	// Dessin
	
	if(instance_->faire_clear_accum_) {
		glClear(GL_ACCUM_BUFFER_BIT);
		instance_->faire_clear_accum_ = false;
	}
	
	instance_->fenetre_->activate();
	instance_->fenetre_->frameInit();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(instance_->motion_blur_) {
		glAccum(GL_MULT, 0.8f);
	}
	
	// Dessin des viewports
	instance_->viewport_->render(instance_->render_action_);
	
	if(instance_->motion_blur_) {
		glAccum(GL_ACCUM, 1 - 0.8f);
		glAccum(GL_RETURN, 1.f);
	}
	
	instance_->fenetre_->swap();
	instance_->fenetre_->frameExit();
}

void gestionnaire_de_scene::timouze(int) {
	glutTimerFunc(instance_->ms_par_frame_ / 2, gestionnaire_de_scene::timouze, 0);
	instance_->temps_courant_ = glutGet(GLUT_ELAPSED_TIME);
 	instance_->temps_depuis_trame_precedente_ = instance_->temps_courant_ - instance_->temps_precedent_;
	if(instance_->temps_depuis_trame_precedente_ > instance_->ms_par_frame_) {
		// Une trame est passée, agissons !
		std::vector<toad*>::iterator it = instance_->toads_a_biper_.begin();
		for(; it != instance_->toads_a_biper_.end(); ++it)
			(*it)->bip(instance_->temps_courant_);
		idle();
		instance_->temps_precedent_ = instance_->temps_courant_;
	}
}

void gestionnaire_de_scene::idle(void) {
	// Animation personnalisée
	instance_->animation();
	
	glutPostRedisplay();
}

void gestionnaire_de_scene::mouvement_souris(int x, int y) {
	instance_->mvt_souris(x,y);
	
	glutPostRedisplay();
}

void gestionnaire_de_scene::bouton_souris(int bouton, int etat, int x, int y) {
	instance_->souris(bouton, etat, x, y);
	
	glutPostRedisplay();
}

void gestionnaire_de_scene::executer() const { 
	glutMainLoop();
}

void gestionnaire_de_scene::attacher(OSG::NodePtr noeud) {
	beginEditCP(noeud_depart_, Node::ChildrenFieldMask);
	{
		noeud_depart_->addChild(noeud);
	}
	endEditCP(noeud_depart_, Node::ChildrenFieldMask);
}

NodePtr gestionnaire_de_scene::init_root() {
	noeud_depart_ = Node::create();
	beginEditCP(noeud_depart_, Node::CoreFieldMask);
	{
		noeud_depart_->setCore(Group::create());
	}
	endEditCP(noeud_depart_, Node::CoreFieldMask);
	
	// Mise en place des beacons
	cam_beacon_ = Node::create();
	light_beacon_ = Node::create();
	beginEditCP(cam_beacon_);
	// beginEditCP(light_beacon_);
		cam_transform_ = Transform::create();
		// light_transform_ = Transform::create();
		beginEditCP(cam_transform_);
		// beginEditCP(light_transform_);
		Matrix M; //, lightM;
			M.setTranslate(Vec3f(x_,y_,z_));
			M.setRotate(Quaternion(Vec3f(rx_,ry_,rz_), rtheta_));
			cam_transform_->setMatrix(M);
			// lightM.setTransform(Vec3f(10, 10, 100));
			// light_transform_->setMatrix(lightM);
		endEditCP(cam_transform_);
		// endEditCP(light_transform_);
		cam_beacon_->setCore(cam_transform_);
		// light_beacon_->setCore(light_transform_);
	endEditCP(cam_beacon_);
	// endEditCP(light_beacon_);
	
	// Mise en place de la caméra
	beginEditCP(camera_);
	{
		camera_->setBeacon(cam_beacon_);
		camera_->setFov(deg2rad(angle_focale_));
		camera_->setNear(0.1);
		camera_->setFar(100);
	}
	endEditCP(camera_);
	
	// Création d'une lumière super cool
	DirectionalLightPtr d_light = DirectionalLight::create();
	beginEditCP(d_light);
	{
		d_light->setDirection(Vec3f(0, 0, 1));
		d_light->setDiffuse(Color4f(1, 1, 1, 1));
		d_light->setAmbient(Color4f(0.8, 0.8, 0.8, 1));
		d_light->setSpecular(Color4f(1, 1, 1, 1));
		d_light->setBeacon(cam_beacon_);
	}
	endEditCP(d_light);
	// PointLightPtr d_light = PointLight::create();
	// 	beginEditCP(d_light);
	// 	{
	// 		d_light->setConstantAttenuation(1);
	// 		d_light->setLinearAttenuation(0);
	// 		d_light->setQuadraticAttenuation(0);
	// 		
	// 		d_light->setDiffuse(Color4f(1,1,1,1));
	// 		d_light->setAmbient(Color4f(0.2, 0.2, 0.2, 1));
	// 		d_light->setSpecular(Color4f(1,1,1,1));
	// 		
	// 		d_light->setBeacon(light_beacon_);
	// 	}
	// 	endEditCP(d_light);
	
	// Création d'un nœud pour la lumière trop cool
	NodePtr noeud_lumiere = Node::create();
	beginEditCP(noeud_lumiere);
	{
		noeud_lumiere->setCore(d_light);
	}
	endEditCP(noeud_lumiere);
	
	// Création du nœud root
	NodePtr scene = Node::create();
	beginEditCP(scene);
	{
		scene->setCore(Group::create());
	}
	endEditCP(scene);
	
	beginEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);
	{
		scene->addChild(noeud_lumiere);
		// scene->addChild(noeud_depart_);
		scene->addChild(cam_beacon_);
		// scene->addChild(light_beacon_);
	}
	endEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);
	
	beginEditCP(noeud_lumiere);
	noeud_lumiere->addChild(noeud_depart_);
	endEditCP(noeud_lumiere);
	
	return scene;
}
