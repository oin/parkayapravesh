#include "gestionnaire_de_scene.h"
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGGLUT.h>
#include <cstdlib>

OSG_USING_NAMESPACE;

gestionnaire_de_scene* gestionnaire_de_scene::instance_ = 0;

gestionnaire_de_scene::gestionnaire_de_scene(int* argc, char** argv) : coords_initiales_(0, 3.5, 10, 0, 0, 0), coords_(0, 3.5, 10, 0, 0, 0) {
	init(argc, argv);
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
}

NodePtr gestionnaire_de_scene::creer_noeud_de_depart() {
	return makeTorus( .5, 2, 16, 32 );//Node::create();
}

void gestionnaire_de_scene::doit_quitter() {
	OSG::osgExit();
	exit(0);
}

gestionnaire_de_scene::~gestionnaire_de_scene() {
	instance_ = 0;
}

int gestionnaire_de_scene::setupGLUT(int* argc, char **argv) {
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	
	int id_fenetre = glutCreateWindow(titre().c_str());
	
	glutDisplayFunc(gestionnaire_de_scene::affichage_fenetre);
	glutIdleFunc(gestionnaire_de_scene::idle);
	glutReshapeFunc(gestionnaire_de_scene::redimensionnement_fenetre);
	glutMotionFunc(gestionnaire_de_scene::mouvement_souris);
	glutMouseFunc(gestionnaire_de_scene::bouton_souris);
	glutKeyboardFunc(gestionnaire_de_scene::clavier);
	
	return id_fenetre;
}

void gestionnaire_de_scene::clavier(unsigned char key, int x, int y) {
	if(instance_->frappe_clavier(key,x,y)) {
		// Action par défaut
		if(key == 27) { // ECHAP
			instance_->doit_quitter();
		}
	}
	
	glutPostRedisplay();
}

void gestionnaire_de_scene::redimensionnement_fenetre(int w, int h) {
	instance_->fenetre_->resize(w, h);
	glutPostRedisplay();
}

void gestionnaire_de_scene::affichage_fenetre(void) {
	// Dessin
	instance_->fenetre_->render(instance_->render_action_);
}

void gestionnaire_de_scene::idle(void) {
	static std::clock_t temps_avant = std::clock();
	// Mise à jour des paramètres
	instance_->coords_.tx+= static_cast<float>((double)(std::clock() - temps_avant) / CLOCKS_PER_SEC >= 1 ? 1 : 0);
	// Déplacement de la camera
	Matrix M;
	beginEditCP(instance_->cam_transform_);
	{
		M.setTransform(instance_->coords_.t());
		instance_->cam_transform_->setMatrix(M);
	}
	endEditCP(instance_->cam_transform_);
	
	// Animation personnalisée
	instance_->animation();
	
	glutPostRedisplay();
	
	// Mise à jour du temps
	temps_avant = std::clock();
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
	beginEditCP(light_beacon_);
		cam_transform_ = Transform::create();
		light_transform_ = Transform::create();
		beginEditCP(cam_transform_);
		beginEditCP(light_transform_);
			Matrix M, lightM;
			M.setTransform(coords_.t());
			cam_transform_->setMatrix(M);
			lightM.setTransform(Vec3f(1, 10, 2));
			light_transform_->setMatrix(lightM);
		endEditCP(cam_transform_);
		endEditCP(light_transform_);
		cam_beacon_->setCore(cam_transform_);
		light_beacon_->setCore(light_transform_);
	endEditCP(cam_beacon_);
	endEditCP(light_beacon_);
	
	// Mise en place de la caméra
	beginEditCP(camera_);
	{
		camera_->setBeacon(cam_beacon_);
		camera_->setFov(deg2rad(90));
		camera_->setNear(0.1);
		camera_->setFar(100);
	}
	endEditCP(camera_);
	
	// Création d'une lumière super cool
	DirectionalLightPtr d_light = DirectionalLight::create();
	beginEditCP(d_light);
	{
		d_light->setDirection(Vec3f(0, 1, 2));
		d_light->setDiffuse(Color4f(1, 1, 1, 1));
		d_light->setAmbient(Color4f(0.2, 0.2, 0.2, 1));
		d_light->setSpecular(Color4f(1, 1, 1, 1));
		d_light->setBeacon(light_beacon_);
	}
	endEditCP(d_light);
	
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
		scene->addChild(noeud_depart_);
		scene->addChild(cam_beacon_);
		scene->addChild(light_beacon_);
	}
	endEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);
	
	return scene;
}