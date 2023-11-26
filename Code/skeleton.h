//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: skeleton.cpp
// Parent: app ?
// Date: 10/10/23
// Content: Handle the armature, and bones component
//**********************************************************************************//
#ifndef SKELETON_H
#define SKELETON_H

//******** LIBRARY ******** 
#include "tools.h"
#include "timeline.h"
#include "bonesinfo.h"
#include "bone.h"
class Skeleton
{
  public:
    Skeleton();
    Skeleton(std::string project);
    ~Skeleton();

    Bone Root_Bone; // Ou faire ccarrélement la liste des Bones, ca sera plus simple pour 
                    // parcourrir tous les bones une fois qu'ils existent

    // J'ai envie de choisir le renderer dans la class skeleton, pour choisir la manière dont on souhaite qu'il s'affiche, indépendamment des muscles et skin
    // Renderer renderer; 

    void draw_skeleton_stick();
    void draw_skeleton_mesh(bool wire_mode=false);
    void set_time(float time);

  private:
    Timeline *_time; //

  // Variable
  private:
    // OpenGL Buffers
    unsigned int VAO, VBO, EBO;
    // Number of faces to render
    int n_faces;

};
#endif // !SKELETON_H
