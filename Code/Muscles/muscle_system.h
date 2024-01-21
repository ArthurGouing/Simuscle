//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: muscles_system.cpp
// Parent: main.cpp
// Date: 26/11/23
// Content: handle all the muscles of the system
//**********************************************************************************//
#ifndef MUSCLES_SYSTEM_H
#define MUSCLES_SYSTEM_H

//******** LIBRARY ******** 
// Standard read/write
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>
#include <utility> // for std::move

// File
#include "muscle.h"
#include "curve.h"
#include "material_property.h"
#include "Render/renderer.h"
#include "Skeleton/skeleton.h"


class MuscleSystem
{
  public:
    // Init functions
    MuscleSystem(std::string project, MatcapRenderer<Geometry>* geom_renderer,
                                      MatcapRenderer<GeometryInterpo>* interpo_renderer,
                                      MatcapRenderer<Curve>* crv_renderer,
                                      Skeleton *skel
                );
    void read_muscles_parameters(std::string muscle_file, std::string project);
    glm::vec3 read_point(std::ifstream& info);

    // Muscle computation
    void solve(int frame);

    // Get/Set functions
    void set_mode(std::string mode) {render_mode = mode;};
    
    // UI functions
    void UI_pannel();

    ~MuscleSystem();

  public:
    // Other class link
    MatcapRenderer<Geometry> *_geom_renderer; // Est ce qu'on mettrait pas un GeomRenderer directement ?
    MatcapRenderer<GeometryInterpo> *_interpo_renderer; // Est ce qu'on mettrait pas un GeomRenderer directement ?
    MatcapRenderer<Curve> *_crv_renderer; // Est ce qu'on mettrait pas un GeomRenderer directement ?
    Skeleton *_skel;

    // Liste of muscles
    std::vector<Muscle> muscles;
    Solver_param solver_param;

    // VBO for the muscles curves
    unsigned int crv_VAO, crv_VBO, crv_EBO;
    std::vector<glm::vec3> values_crv;
    std::vector<int>       indices_crv;
    int n_values_crv;

    // ????
    int n_point;
    int _nb_frames;
    
    // UI variables
    int line_width, point_width;
    bool gravity;

    // Other parameters
    std::string render_mode; // choice: ["stick", "wire", "mesh"]
};

#endif // !MUSCLES_SYSTEM_H
