#ifndef _DIEU_H
#define _DIEU_H

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGSimpleGeometry.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGGradientBackground.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include <OpenSG/OSGBaseFunctions.h>
#include <OpenSG/OSGTransform.h>
#include <OpenSG/OSGVecFieldDataType.h>
#include "gestionnaire_de_scene.h"

class Dieu {

public : 

  Dieu ();
  ~Dieu();

  OSG::Vec3f getPosition();
  void setPosition(OSG::Vec3f pos);
  OSG::Vec3f getRotation();
  void setRotation(OSG::Vec3f rot);
  OSG::NodePtr getIncarnationCourante();
  OSG::NodePtr getSelectionCourante();
  void deplacer(float x,float y, float vitesse);
  void aller_a (float x, float y);
  OSG::NodePtr selectionner(float x,float y);
  void incarner();
  void desincarner();


private : 
  OSG::Vec3f position;
  OSG::Vec3f rotation;
  OSG::NodePtr incarnation_courante;
  OSG::NodePtr selection_courante;

};

#endif //_DIEU_H
