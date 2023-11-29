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

// File
#include "muscle.h"
#include "Skeleton/skeleton.h"

class MuscleSystem
{
  public:
    MuscleSystem(std::string project, Skeleton *skel);
    ~MuscleSystem();

    // Functions
    void read_muscles_parameters(std::string muscle_file, std::string project);

    void update_VBO();

    void draw_muscles();

    // Get/Set functions
    void set_mode(std::string mode) {render_mode = mode;};
  private:
    // Other class link
    Skeleton *_skel;

    // Buffers
    unsigned int VAO, VBO, EBO;
    int n_values;

    // Liste of muscles
    std::vector<Muscle> muscles;

    // Other parameters
    std::string render_mode; // choice: ["stick", "wire", "mesh"]
};

#endif // !MUSCLES_SYSTEM_CPP
