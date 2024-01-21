//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: curve.cpp
// Parent: muscle.cpp
// Date: 18/01/24
// Content: Class that contain the skin class (geometry and deformations)
//**********************************************************************************//
#ifndef SKIN_H
#define SKIN_H

//******** LIBRARY ******** 
// Standard read/write
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// Other files
#include "Render/renderer.h"
#include "Skeleton/skeleton.h"
#include "Muscles/muscle_system.h"
#include "Geometry/geometry.h"

class Skin 
{
  public:
    // Constructor
    Skin(std::string project, MatcapRenderer<Geometry>* skin_renderer, Skeleton* skel, MuscleSystem* musc);

    // Init functions
    void read_weights(std::string filename);
    //

    // Compute skin deformations
    void solve(int frame);

    void UI_pannel();

    ~Skin();

  public:
    std::string name;

  private:
    // Other class link
    Skeleton*     _skel;
    MuscleSystem* _musc;
    
    Geometry _mesh;
	  std::vector<std::vector<float> >  weights;	 // Integreate in a new geometry class !

    std::vector<int> fixed_point; // list of vertex that follow won't be simulated
                                  // i.e. which will follow the skeleton animation

    MatcapRenderer<Geometry>* _renderer;
};

#endif // !SKIN_H
