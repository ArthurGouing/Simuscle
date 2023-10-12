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

class AnimCurve
{
  public:
    AnimCurve()  {};
    ~AnimCurve() {_values.clear();};

    double get_values(int frame)  {/*TODO*/};
    double get_values(float time) {/*TODO*/};
  public:
    std::string name; // Name of the dof
    std::vector<double> _values; // values for all the frames

};

enum RotateOrder {roXYZ=123, roYZX=312, roZXY=231, roXZY=132, roYXZ=213, roZYX=321};

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


    std::string _name;
   private:
    Geometry _mesh;
    glm::vec3 _offset; // peut etre en vec4 pour pouvoir faire les transform
    glm::vec3 _translation;
    glm::vec3 _rotation;
    RotateOrder _rorder;

    std::vector<Bone> _childrens;
    int _nb_frames; // a mettre dans timeline plutot
    int _frame_time;// a mettre dans timeline plutot

    // glm::mat4 transformation;
    // glm::mat4 trans_only_rot;
    // glm::vec3 translation_2;
    // glm::vec3 offset_2;

    std::vector<AnimCurve> _dofs; // Class which Store the value of all the frame, and compute the value at the interframe.
};

#endif // !BONE_H
