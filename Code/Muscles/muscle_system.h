//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: muscles_system.cpp
// Parent: main.cpp
// Date: 26/11/23
// Content: handle all the muscles of the system
//**********************************************************************************//
#ifndef MUSCLES_SYSTEM_CPP
#define MUSCLES_SYSTEM_CPP

//******** LIBRARY ******** 
// Standard read/write
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/glm.hpp>

// File
#include "muscle.h"
#include "curve.h"
#include "Skeleton/skeleton.h"

class MuscleSystem
{
  public:
    MuscleSystem(std::string project, Skeleton *skel);
    ~MuscleSystem();

    // Functions
    void read_muscles_parameters(std::string muscle_file, std::string project);
    glm::vec3 read_point(std::ifstream& info);

    void update_VBO();

    void draw_muscles();
    void draw_curves(int frame);

    // Get/Set functions
    void set_mode(std::string mode) {render_mode = mode;};
  private:
    // Other class link
    Skeleton *_skel;

    // Buffers
    unsigned int VAO, VBO, EBO;
    unsigned int crv_VAO, crv_VBO, crv_EBO;
    int n_values, n_point;

    // Liste of muscles
    std::vector<Muscle> muscles;

    // Other parameters
    std::string render_mode; // choice: ["stick", "wire", "mesh"]
};

#endif // !MUSCLES_SYSTEM_CPP
