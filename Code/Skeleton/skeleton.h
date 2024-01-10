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
 #include "Render/renderer.h"
#include "Tools/tools.h"
#include "timeline.h"
#include "bonesinfo.h"
#include "bone.h"
class Skeleton
{
  public:
    Skeleton(); // TODO: useless constructor
    Skeleton(std::string project, Renderer* renderer);
    ~Skeleton();

    Renderer* renderer; // Needed to link geometry and update VAO
    Bone Root_Bone; // Ou faire ccarrélement la liste des Bones, ca sera plus simple pour 
                    // parcourrir tous les bones une fois qu'ils existent

    void compute(int frame); // Il manque un trux après compute, mais je sais pas quoi mettre
 
    void draw_skeleton_stick();
    void draw_skeleton_mesh(bool wire_mode=false);

    Bone* find_bone(std::string bone_name);

    // Set/Get functions
    void set_mode(std::string mode) {render_mode = mode;};
    void set_time(float time);

    void UI_pannel();
  private:
    Timeline *_time; //

  // Variable
  public:
    // number of frame
    int _nb_frames;

  private:
    // The mode the way the skeleton is rendered
    std::string render_mode; // choice: ["stick", "wire", "mesh"]

    // Geometry vector
    std::vector<glm::vert_arr> values;
    std::vector<int>           indices;

    // OpenGL Buffers
    unsigned int VAO, VBO, EBO;
    // Number of faces to render
    int n_values;

    // parmeters
    bool reset_pose;

};
#endif // !SKELETON_H
