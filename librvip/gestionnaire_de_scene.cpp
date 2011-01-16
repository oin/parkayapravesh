#include "gestionnaire_de_scene.h"
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGPointLight.h>
#include <OpenSG/OSGPassiveViewport.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGImageForeground.h>
#include <OpenSG/OSGFileGrabForeground.h>
#include <OpenSG/OSGSimpleAttachments.h>
#include <OpenSG/OSGImage.h>
#include <OpenSG/OSGComponentTransform.h>
#include <cstdlib>
#include <ctime>

OSG_USING_NAMESPACE;

gestionnaire_de_scene* gestionnaire_de_scene::instance_ = 0;

gestionnaire_de_scene::gestionnaire_de_scene(int* argc, char** argv, std::string titre) : titre_(titre), x_(0), y_(3.5), z_(10), angle_focale_(90), fps_(50), ms_par_frame_(static_cast<int>(1.0/fps_ * 1000)), motion_blur_(false) {
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
	
	// Création des camera
	camera_ = PerspectiveCamera::create();
	camera2_ = PerspectiveCamera::create();
	
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
	
	GradientBackgroundPtr bkg2 = GradientBackground::create();
	beginEditCP(bkg2);
	{
		bkg2->addLine(Color3f(0.5,0.5,0.5), 0);
		bkg2->addLine(Color3f(0.1,0.1,0.1), 1);
	}
	endEditCP(bkg2);
	
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
	
		// Mise en place d'un viewport
	viewport2_ = Viewport::create();
	beginEditCP(viewport2_);
	{
		viewport2_->setCamera(camera2_);
		viewport2_->setRoot(noeud_root_);
		viewport2_->setBackground(bkg2);
		viewport2_->setSize(0.8,0,1,0.2);
	}
	endEditCP(viewport2_);
	
	// Ajoute le viewport 3ieme personne à la fenêtre
	fenetre_->addPort(viewport2_);
	
	iconeFermer();
	// cadre();
    
	// Crée une action de rendu
	render_action_ = RenderAction::create();
	render_action_->setWindow(fenetre_.getCPtr());
	
	// Crée des doigts
	ImagePtr doigtimg = Image::create();
	doigtimg->read("data/doigt.png");

	doigt1_ = ImageForeground::create();
	beginEditCP(doigt1_);
	    doigt1_->addImage(doigtimg,Pnt2f(0,0));
	endEditCP(doigt1_);
	doigt2_ = ImageForeground::create();
	beginEditCP(doigt2_);
	    doigt2_->addImage(doigtimg,Pnt2f(0,0));
	endEditCP(doigt2_);
	
	viewport_->getMFForegrounds()->push_back(doigt1_);
	viewport_->getMFForegrounds()->push_back(doigt2_);
	
	
	// Crée le fond pour l'incarnation
	ImagePtr fondimg = Image::create();
	fondimg->read("data/fond_incarnation.png");

	fond_incarnation_ = ImageForeground::create();
	beginEditCP(fond_incarnation_);
	    fond_incarnation_->addImage(fondimg,Pnt2f(0,1.0));
	endEditCP(fond_incarnation_);
	
	viewport_->getMFForegrounds()->push_back(fond_incarnation_);
	
	// Crée le cadre
	ImagePtr cadreimg1 = Image::create();
	cadreimg1->read("data/cadre_haut.png");

	cadre_haut_ = ImageForeground::create();
	beginEditCP(cadre_haut_);
	    cadre_haut_->addImage(cadreimg1,Pnt2f(0.0,0.992));
	endEditCP(cadre_haut_);
	
	viewport2_->getMFForegrounds()->push_back(cadre_haut_);
	
	ImagePtr cadreimg2 = Image::create();
	cadreimg2->read("data/cadre_gauche.png");

	cadre_gauche_ = ImageForeground::create();
	beginEditCP(cadre_gauche_);
	    cadre_gauche_->addImage(cadreimg2,Pnt2f(0.0,0.0));
	endEditCP(cadre_gauche_);
	
	viewport2_->getMFForegrounds()->push_back(cadre_gauche_);
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

void gestionnaire_de_scene::camera_move_y(double dy) {
	Vec3f sup = navigateur_.getUp();
	sup.normalize();
	sup *= dy;
	Matrix M;
	M.setIdentity();
	M.setTranslate(sup);
	Pnt3f nfrom, nat;
	M.mult(navigateur_.getFrom(), nfrom);
	M.mult(navigateur_.getAt(), nat);
	navigateur_.setFrom(nfrom);
	navigateur_.setAt(nat);
}

void gestionnaire_de_scene::camera_rotation(double x, double y) {
	navigateur_.rotate(x, y);
	Vec3f up = cam_up_; // navigateur_.getUp();
	Quaternion q;
	Vec3f sv = navigateur_.getAt() - navigateur_.getFrom();
	sv.normalize();
	sv.crossThis(up);
	sv.normalize();
	q.setValueAsAxisRad(sv, -y);
	Matrix M;
	M.setIdentity();
	q.getValue(M);
	M.mult(up, up);
	cam_up_ = up;
}

void gestionnaire_de_scene::placer_camera() {
	// Focale de la camera
	beginEditCP(camera_);
		camera_->setFov(deg2rad(angle_focale_));
	endEditCP(camera_);
	
	beginEditCP(camera2_);
		camera2_->setFov(deg2rad(angle_focale_));
		//camera2_->setNear( 5 );
		//camera2_->setFar( 500 );
	endEditCP(camera2_);
	
	cam_transform_->setMatrix(navigateur_.getMatrix());
	
	if(incarnation_ != NullFC) {
		FlyNavigator n;
		n.set(navigateur_.getMatrix());
		n.setFrom(navigateur_.getFrom());
		n.setAt(navigateur_.getAt());
		n.forward(taille_y_inc_ * 2.2);
		cam2_transform_->setMatrix(n.getMatrix());
	}
}

void gestionnaire_de_scene::selection_move(double x, double y, double z) {
	static const double q = 0.6;
	
	// Translation sur le vecteur side de la caméra
	Vec3f vs = navigateur_.getFrom() - navigateur_.getAt();
	vs.crossThis(navigateur_.getUp());
	vs.normalize();
	vs *= x * q;
	
	// Translation sur le vecteur up de la caméra
	Vec3f vup = navigateur_.getUp();
	vup.normalize();
	vup *= y * q;
	
	// Translation sur le vecteur forward de la caméra
	Vec3f vf = navigateur_.getAt() - navigateur_.getFrom();
	vf.normalize();
	vf *= z * q;
	Matrix tf;
	tf.setIdentity();
	tf.setTranslate(vs + vup + vf);
	
	sel_mat_mvt_ = tf;
}

void gestionnaire_de_scene::selection_rotate(double x, double y) {
	// Rotation autour du vecteur up de la caméra
	Vec3f vup = cam_up_;
	Quaternion rot_up;
	rot_up.setValueAsAxisRad(cam_up_, -x);
	
	Matrix mat;
	mat.setIdentity();
	rot_up.getValue(mat);
	
	// Rotation autour du vecteur side de la caméra
	Vec3f vs = navigateur_.getAt() - navigateur_.getFrom();
	vs.normalize();
	vs.crossThis(vup);
	vs.normalize();
	Quaternion rot_side;
	rot_side.setValueAsAxisRad(vs, y);
	
	Matrix m2;
	m2.setIdentity();
	rot_side.getValue(m2);
	
	mat.mult(m2);
	sel_mat_rot_ = mat;
}

void gestionnaire_de_scene::animation() {
	placer_camera();
	// Bouge la sélection
	if(selection_ != NullFC) {
		beginEditCP(selection_);
		ComponentTransformPtr p = ComponentTransformPtr::dcast(selection_->getCore());
		Matrix M;
		M = sel_mat_mvt_;
		// M = p->getMatrix();
		M.mult(p->getMatrix());
		M.mult(sel_mat_rot_);
		p->Transform::setMatrix(M);
		endEditCP(selection_);
		sel_mat_rot_.setIdentity();
		sel_mat_mvt_.setIdentity();
	}
	// Bouge les doigts
	if(doigt1_ != NullFC) {
		doigt1_->editActive() = doigt1_set_;
		doigt1_->editPositions(0).setValue(Pnt2f(doigt1_x_, 1 - doigt1_y_));
	}
	if(doigt2_ != NullFC) {
		doigt2_->editActive() = doigt2_set_;
		doigt2_->editPositions(0).setValue(Pnt2f(doigt2_x_, 1 - doigt2_y_));
	}
	// Bouge l'objet incarné
	if(incarnation_ != NullFC) {
		fond_incarnation_->editActive() = true;
		FlyNavigator n;
		n.set(navigateur_.getMatrix());
		n.setFrom(navigateur_.getFrom());
		n.setAt(navigateur_.getAt());
		n.forward(taille_y_inc_);
		
		beginEditCP(incarnation_);
		ComponentTransformPtr p = ComponentTransformPtr::dcast(incarnation_->getCore());
		p->Transform::setMatrix(n.getMatrix());
		endEditCP(incarnation_);
		// inc_mat_ = n.getMatrix();
	} else {
		fond_incarnation_->editActive() = false;
	}
	// Fait l'animation de désincarnation
	if(compteur_desincarnation_ > 0) {
		if(!motion_blur_)
			motion_blur(true);
		--compteur_desincarnation_;
	} else if(compteur_desincarnation_ == 0) {
		motion_blur(false);
		glClearColor(0.0, 0.0, 0.0, 0.0);
		compteur_desincarnation_ = -1;
	}
}

void gestionnaire_de_scene::incarner() {
	if(selection_ == NullFC || incarnation_ != NullFC)
		return;
	
	DynamicVolume vol;
	Pnt3f m, M;
	selection_->getWorldVolume(vol);
	vol.getBounds(m, M);
	taille_y_inc_ = M[1] - m[1];
	taille_z_inc_ = M[2] - m[2];
	
	// Pnt3f f = navigateur_.getFrom();
	// Pnt3f a = navigateur_.getAt();
	
	// f[1] += taille_y_inc_;
	// a[1] += taille_y_inc_;
	
	// navigateur_.setFrom(f);
	// navigateur_.setAt(a);
	
	incarnation_ = selection_;

	selection_ = NullFC;
	geonode_selection_ = NullFC;
	highlightChanged();
}

void gestionnaire_de_scene::desincarner() {
	if(incarnation_ == NullFC)
		return;
	
	// Place la caméra 1 à la place de la caméra 2
	navigateur_.forward(taille_y_inc_ * 2.2);
	
	incarnation_ = NullFC;
	
	// Truc de ouf
	glClearColor(1.0, 1.0, 1.0, 1.0);
	faire_clear_accum_ = true;
	compteur_desincarnation_ = 10;
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
	glutSetCursor(GLUT_CURSOR_NONE);
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
		} //else if(key == 'b')
			//instance_->toggle_motion_blur();
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
	if(instance_->incarnation_ != NullFC)
		instance_->viewport2_->render(instance_->render_action_);
	
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
	instance_->updateHighlight();
	
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
	cam_beacon3_ = Node::create();
	cam_beacon2_ = Node::create();
	cam_beacon_ = Node::create();
	cam2_beacon_ = Node::create();
	light_beacon_ = Node::create();
	cam_transform_ = Transform::create();
	cam2_transform_ = Transform::create();
	cam_transform2_ = Transform::create();
	cam_transform3_ = Transform::create();
	
	beginEditCP(cam_beacon3_);
		cam_beacon3_->setCore(cam_transform3_);
	endEditCP(cam_beacon3_);
	
	beginEditCP(cam_beacon2_);
		cam_beacon2_->setCore(cam_transform2_);
		cam_beacon2_->addChild(cam_beacon3_);
	endEditCP(cam_beacon2_);
	
	beginEditCP(cam_beacon_);		
		cam_beacon_->setCore(cam_transform_);
		cam_beacon_->addChild(cam_beacon2_);
	endEditCP(cam_beacon_);

	placer_camera();
	
	// Mise en place de la caméra
	beginEditCP(camera_);
	{
		camera_->setBeacon(cam_beacon_);
		camera_->setFov(deg2rad(angle_focale_));
		camera_->setNear(0.1);
		camera_->setFar(1000);
	}
	endEditCP(camera_);
	

	// Mise en place de la caméra 3ieme personne
	beginEditCP(cam2_beacon_);		
		cam2_beacon_->setCore(cam2_transform_);
		cam2_beacon_->addChild(cam_beacon2_);
	endEditCP(cam2_beacon_);

	beginEditCP(camera2_);
	{
		camera2_->setBeacon(cam2_beacon_);
		camera2_->setFov(deg2rad(angle_focale_));
		camera2_->setNear(0.1);
		camera2_->setFar(100);
		/*camera2_->setNear( 0.5 );
		camera2_->setFar( 8000 );
		*/

	}
	endEditCP(camera2_);

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
		scene->addChild(cam_beacon_);
		scene->addChild(cam2_beacon_);
	}
	endEditCP(scene, Node::CoreFieldMask | Node::ChildrenFieldMask);
	
	beginEditCP(noeud_lumiere);
	noeud_lumiere->addChild(noeud_depart_);
	endEditCP(noeud_lumiere);
	
	navigateur_.setFrom(Pnt3f(x_,y_,z_));
	cam_up_ = navigateur_.getUp();
	
	return scene;
}

bool gestionnaire_de_scene::point_projete(double x, double y, Pnt3f& pnt) {
	Line rayon;
	camera_->calcViewRay(rayon, static_cast<osg::Int32>(x * viewport_->getPixelWidth()), static_cast<osg::Int32>(y * viewport_->getPixelHeight()), *viewport_);
	IntersectAction *int_act = IntersectAction::create();
	int_act->setLine(rayon);
	int_act->apply(noeud_root_);
	if(int_act->didHit()) {
		pnt = int_act->getHitPoint();
		return true;
	}
	return false;
}

bool gestionnaire_de_scene::est_sur_selection(double x, double y) {
	Line rayon;
	if(!camera_ || !viewport_)
		return false;
	beginEditCP(camera_);
	beginEditCP(viewport_);
	camera_->calcViewRay(rayon, static_cast<osg::Int32>(x * viewport_->getPixelWidth()), static_cast<osg::Int32>(y * viewport_->getPixelHeight()), *viewport_);
	IntersectAction *int_act = IntersectAction::create();
	int_act->setLine(rayon);
	int_act->apply(noeud_root_);
	endEditCP(camera_);
	endEditCP(viewport_);
	if(int_act->didHit()) {
		return int_act->getHitObject().getCPtr() == geonode_selection_.getCPtr();
	}
	return false;
}

void gestionnaire_de_scene::selectionner(double x, double y) {
	Line rayon;
	std::string targets_str("targets");
	camera_->calcViewRay(rayon, static_cast<osg::Int32>(x * viewport_->getPixelWidth()), static_cast<osg::Int32>(y * viewport_->getPixelHeight()), *viewport_);
	IntersectAction *int_act = IntersectAction::create();
	int_act->setLine(rayon);
	int_act->apply(noeud_root_);
	if(int_act->didHit()) {
		NodePtr truc_touche = int_act->getHitObject();
		NodePtr geo_touche = truc_touche;
		dernier_hit_point_selection_ = int_act->getHitPoint();
		// C'est un Geometry
		if(truc_touche->getCore()->getTypeId() == 410) {
			NodePtr parent_truc = truc_touche;
			const Char8* parent_name;
			for(size_t i=0; i<5; ++i) {
				truc_touche = parent_truc;
				if(truc_touche == NullFC)
					return;
				parent_truc = truc_touche->getParent();
				if(parent_truc != NullFC) {
					parent_name = getName(parent_truc);
					if(truc_touche->getCore()->getTypeId() == 289 && parent_name && parent_name == targets_str) {
						if(truc_touche->getNChildren() == 1 && truc_touche->getChild(0) != NullFC && truc_touche->getChild(0)->getCore()->getTypeId() == 289)
							truc_touche = truc_touche->getChild(0);
						// Sélection qui change !!
						selection_ = truc_touche;
						geonode_selection_ = geo_touche;
						highlightChanged();
						ComponentTransformPtr p =  ComponentTransformPtr::dcast(selection_->getCore());
						sel_mat_ = p->getMatrix();
						return;
					}
				}
			}
		}
	}
	
	// Pas de sélection
	selection_ = NullFC;
	geonode_selection_ = NullFC;
	highlightChanged();
	sel_mat_.setIdentity();
}

void gestionnaire_de_scene::highlightChanged() {
	// init as needed
	if(_highlightMaterial == NullFC)
	{
		_highlightMaterial = SimpleMaterial::create();

		beginEditCP(_highlightMaterial);
		_highlightMaterial->setDiffuse (Color3f(1,0.1,0.1));
		_highlightMaterial->setLit     (false);
		endEditCP(_highlightMaterial);
	}
	if(_highlightNode == NullFC)
	{
		GeoPTypesPtr type = GeoPTypesUI8::create();
		beginEditCP(type);
		type->push_back(GL_LINE_STRIP);
		type->push_back(GL_LINES);
		endEditCP(type);

		GeoPLengthsPtr lens = GeoPLengthsUI32::create();
		beginEditCP(lens);
		lens->push_back(10);
		lens->push_back(6);
		endEditCP(lens);

		GeoIndicesUI32Ptr index = GeoIndicesUI32::create();
		beginEditCP(index);

		index->getFieldPtr()->push_back(0);
		index->getFieldPtr()->push_back(1);
		index->getFieldPtr()->push_back(3);
		index->getFieldPtr()->push_back(2);
		index->getFieldPtr()->push_back(0);
		index->getFieldPtr()->push_back(4);
		index->getFieldPtr()->push_back(5);
		index->getFieldPtr()->push_back(7);
		index->getFieldPtr()->push_back(6);
		index->getFieldPtr()->push_back(4);

		index->getFieldPtr()->push_back(1);
		index->getFieldPtr()->push_back(5);
		index->getFieldPtr()->push_back(2);
		index->getFieldPtr()->push_back(6);
		index->getFieldPtr()->push_back(3);
		index->getFieldPtr()->push_back(7);
		
		endEditCP(index);

		_highlightPoints = GeoPositions3f::create();
		beginEditCP(_highlightPoints);
		_highlightPoints->push_back(Pnt3f(-1, -1, -1));
		_highlightPoints->push_back(Pnt3f( 1, -1, -1));
		_highlightPoints->push_back(Pnt3f(-1,  1, -1));
		_highlightPoints->push_back(Pnt3f( 1,  1, -1));
		_highlightPoints->push_back(Pnt3f(-1, -1,  1));
		_highlightPoints->push_back(Pnt3f( 1, -1,  1));
		_highlightPoints->push_back(Pnt3f(-1,  1,  1));
		_highlightPoints->push_back(Pnt3f( 1,  1,  1));
		endEditCP(_highlightPoints);

		GeometryPtr geo=Geometry::create();
		beginEditCP(geo);
		geo->setTypes     (type);
		geo->setLengths   (lens);
		geo->setIndices   (index);
		geo->setPositions (_highlightPoints);
		geo->setMaterial  (_highlightMaterial);
		endEditCP(geo);
		addRefCP(geo);

		_highlightNode = Node::create();
		beginEditCP(_highlightNode);
		_highlightNode->setCore(geo);
		endEditCP(_highlightNode);
		addRefCP(_highlightNode);
	}

	// attach the hightlight node to the root if the highlight is active
	if(selection() != NullFC)
	{
		if(_highlightNode->getParent() == NullFC)
		{
			beginEditCP(noeud_root_);
			noeud_root_->addChild(_highlightNode);
			endEditCP(noeud_root_);
		}
	}
	else
	{
		if(_highlightNode->getParent() != NullFC)
		{
			beginEditCP(noeud_root_);
			noeud_root_->subChild(_highlightNode);
			endEditCP(noeud_root_);
		}

	}
	// update the highlight geometry
	updateHighlight();
}

void gestionnaire_de_scene::updateHighlight() {
	if(selection_==NullFC)
		return;

	// calc the world bbox of the highlight object
#ifndef OSG_2_PREP
	 DynamicVolume vol;
#else
	BoxVolume      vol;
#endif
	selection_->getWorldVolume(vol);

	Pnt3f min,max;
	vol.getBounds(min, max);

	beginEditCP(_highlightPoints);
	_highlightPoints->setValue(Pnt3f(min[0], min[1], min[2]), 0);
	_highlightPoints->setValue(Pnt3f(max[0], min[1], min[2]), 1);
	_highlightPoints->setValue(Pnt3f(min[0], max[1], min[2]), 2);
	_highlightPoints->setValue(Pnt3f(max[0], max[1], min[2]), 3);
	_highlightPoints->setValue(Pnt3f(min[0], min[1], max[2]), 4);
	_highlightPoints->setValue(Pnt3f(max[0], min[1], max[2]), 5);
	_highlightPoints->setValue(Pnt3f(min[0], max[1], max[2]), 6);
	_highlightPoints->setValue(Pnt3f(max[0], max[1], max[2]), 7);
	endEditCP(_highlightPoints);

	beginEditCP(_highlightNode->getCore(), Geometry::PositionsFieldMask);
	endEditCP  (_highlightNode->getCore(), Geometry::PositionsFieldMask);
}

void gestionnaire_de_scene::iconeFermer(){
	ImagePtr img = Image::create();
	beginEditCP(img);
		img->read("data/iconeFermer.png");
	endEditCP(img);

	ImageForegroundPtr imgFrg = ImageForeground::create();
    beginEditCP(imgFrg);
        imgFrg->addImage(img,Pnt2f(0,0));
    endEditCP(imgFrg);
    viewport_->getMFForegrounds()->push_back(imgFrg);


}
void gestionnaire_de_scene::cadre(){
	  //Chargement du cadre pour le viewport 3ieme personne
	ImagePtr img = Image::create();
	beginEditCP(img);
		img->read("data/carre.png");
		//img->read("data/iconeFermer.png");
	endEditCP(img);

	ImageForegroundPtr imgFrg = ImageForeground::create();
	beginEditCP(imgFrg);
	    imgFrg->addImage(img,Pnt2f(0,0));
	endEditCP(imgFrg);
	
	cadreViewport = ImageForeground::create();
	beginEditCP(cadreViewport);
	    cadreViewport->addImage(img,Pnt2f(0,0));
	endEditCP(cadreViewport);
	//cadreViewport.scale(10,10);
	viewport2_->getMFForegrounds()->push_back(cadreViewport);
}