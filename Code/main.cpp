//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: main.cpp
// Parent: None
// Date: 14/09/23
// Content: Main file which launch the app
//**********************************************************************************//
#ifndef MAIN_CPP
#define MAIN_CPP

//******** LIBRARY ******** 
#include "app.h"
#include "renderer.h"
#include "geometry.h"

int main()
{
  // Initialize the application
  std::vector<float> vertices = {
       0.5f,  0.5f, 0.5f ,  // up top right
      -0.5f,  0.5f, 0.5f,   // up top left 
      -0.5f, -0.5f, 0.5f,   // up bottom left
       0.5f, -0.5f, 0.5f,   // up bottom right
       0.5f,  0.5f, -0.5f,   // down top right
      -0.5f,  0.5f, -0.5f,   // down top left 
      -0.5f, -0.5f, -0.5f,   // down bottom left
       0.5f, -0.5f, -0.5f    // down bottom right
  };
  std::vector<int> indices = {  // note that we start from 0!
      0, 1, 3,  // Up first Triangle
      1, 2, 3,  // Up second Triangle
      0, 3, 4,  // Right first Triangle
      3, 7, 4,  // Right second Triangle
      1, 0, 4,  // Back first Triangle
      4, 5, 1,  // Back second Triangle
      2, 1, 5,  // Left first Triangle
      5, 6, 2,  // Left second Triangle
      2, 6, 7,  // Front first Triangle
      7, 3, 2,  // Front second Triangle
      5, 4, 7,  // Down first Triangle
      7, 6, 5   // Down second Triangle
  };
  // std::vector<int> indices = {
  //       0, 1, 2, 3 // UP
  //       3, 0, 4, 7, // Right
  //       0, 4, 5, 1, // Back
  //       1, 5, 6, 2, // Left
  //       2, 6, 7, 3, // Front
  //       7, 6, 5, 4, // Down

  // };
  Title_Print("Create Geometry from file");
  // Geometry square(&vertices, &indices);
  Geometry mesh("../Blender_scene/Human_triangulate.off");
  // Geometry mesh("../Blender_scene/Sphere.off");
  Info_Print("Done");
  Renderer basic_render;
  App Simuscle_app(&basic_render, &mesh);
  Simuscle_app.Init();

  // Run the application
  Simuscle_app.Run();

  return 0;
}

// Ca sera plus compliquer quand j'aurais plusieurs mode, et différente phase au cours de l'application
#endif // !MAIN_CPP
