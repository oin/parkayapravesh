#include "dieu.h"

using namespace OSG; 

Dieu ::Dieu(){

}

Dieu :: ~Dieu(){

}

Vec3f Dieu ::getPosition(){
  return position;
}

Vec3f Dieu:: getRotation(){
  return rotation;
}

void Dieu:: setPosition(Vec3f pos){
  position = pos;
}

void Dieu::setRotation(Vec3f rot){
  position = rot;
}

NodePtr Dieu::getIncarnationCourante(){
  return incarnation_courante;
}

NodePtr Dieu::getSelectionCourante(){
  return selection_courante;
}

void Dieu:: deplacer(float x,float y, float vitesse){
  
}

void Dieu::aller_a (float x, float y){

}

NodePtr Dieu::selectionner(float x, float y){
  //return gestionnaire_de_scene::instance().getNoeudSur(x,y);
}

void Dieu::incarner(){

}

void Dieu:: desincarner(){

}
