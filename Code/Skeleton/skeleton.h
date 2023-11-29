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
#include "Tools/tools.h"
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

    void init_buffers();

    void draw_skeleton_stick();
    void draw_skeleton_mesh(bool wire_mode=false);

    Bone* find_bone(std::string bone_name);

    // Set/Get functions
    void set_mode(std::string mode) {render_mode = mode;};
    void set_time(float time);

  private:
    Timeline *_time; //

  // Variable
  private:
    // The mode the way the skeleton is rendered
    std::string render_mode; // choice: ["stick", "wire", "mesh"]
    // OpenGL Buffers
    unsigned int VAO, VBO, EBO;
    // Number of faces to render
    int n_values;

};
#endif // !SKELETON_H
