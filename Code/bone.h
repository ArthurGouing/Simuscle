//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: bone.cpp
// Parent: Skeleton
// Date: 10/10/23
// Content: Create Bones, frome bvh files, and store values...
//**********************************************************************************//
#ifndef BONE_H
#define BONE_H

//******** LIBRARY ******** 
// Standard read/write
#include <iostream>
#include <fstream>
#include <sstream>

// File
#include "tools.h"
#include "geometry.h"


enum RotateOrder {roXYZ=0, roYZX, roZXY, roXZY, roYXZ, roZYX};

class Bone // = The joint class in SIA Project
{
  public:
    Bone();
    Bone(std::string file_name);
    Bone(std::ifstream& anim_file, std::string name, Bone* parent);
    ~Bone();

    void create_from_file(std::string file_name);
   private:
    // static void create(std::string name, glm::vec3 offset, Bone* parent);
    void parse_bone(std::ifstream& anim_file, std::string name, Bone* parent);
    void parse_frames(std::ifstream& anim_file);
    void animate(float time=0); // ou un float et faire avec le time 
    // void getglobaleposition
    // computevertex
    // computeoffset
    void print_bone(int level=0);


   private:
    std::string _name;
    Geometry _mesh;
    glm::vec3 _offset; // peut etre en vec4 pour pouvoir faire les transform
    glm::vec3 _translation;
    glm::vec3 _rotation;
    int _rorder;

    std::vector<Bone*> _childrens;
    int nb_frames; // a mettre dans timeline plutot
    int frame_time;// a mettre dans timeline plutot

    glm::mat4 transformation;
    glm::mat4 trans_only_rot;
    glm::vec3 translation_2;
    glm::vec3 offset_2;

    // AnimCurve dof; // Class which Store the value of all the frame, and compute the value at the interframe.
};

#endif // !BONE_H
