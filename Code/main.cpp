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
#include "skeleton.h"

int main()
{
  // Initialize the application
  // Geometry mesh("../Blender_scene/Human_triangulate.off");
  // Geometry mesh("../Blender_scene/Sphere.off");
  std::string project = "./Test_case/";
  Skeleton skeleton(project);
  // MuscleSystem muscles(project, &skeleton);
  //Skin character(project, &skeleton, &muscles);
  Renderer matcap_render(&skeleton/*, &muscles, &character*/);
  App Simuscle_app(&matcap_render);
  Simuscle_app.Init();

  // Run the application
  Simuscle_app.Run();

  return 0;
}

// Ca sera plus compliquer quand j'aurais plusieurs mode, et différente phase au cours de l'application
#endif // !MAIN_CPP
