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
#include "Render/render_manager.h"
#include "Render/renderer.h"
#include "Skeleton/skeleton.h"
#include "Muscles/muscle_system.h"
// #include "Skin/skin.h"

// Command line parser
#include "Tools/argparse.hpp"

int main(int argc, char *argv[])
{
  /******** Parse argument ********/ // TODO: on lui fera surement une petite class
  argparse::ArgumentParser program("Simuscle", "1.0");
  program.add_argument("project_path").default_value(std::string("../Blender_scene/Arm_Simuscle/"));
  program.add_argument("-v", "--verbose"); // parameter packing
  try {
    program.parse_args(argc, argv);
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }
  // std::string project = "../Blender_scene/Arm_Simuscle/";
  std::string project = program.get<std::string>("project_path");


  /******** Start Simuscle ********/

  Title_Print("Launch Simuscle App (with skeleton)");
  Info_Print("Commit  : ******");
  Info_Print("Modif   : 23/09/23");
  Info_Print("Version : 0.0.0");
  // Create render pipeline
  RenderManager r_manager;
  // Create Renderer
  Renderer *skel_renderer  = new MatcapRenderer<Geometry>("skeleton shader", "skin_shader.vert", "skin_shader.frag", "white_matcap_2.jpg");
  Renderer *musc_renderer  = new MatcapRenderer<Geometry>("muscle shader", "skin_shader.vert", "skin_shader.frag", "red_matcap.png");
  // Renderer *curve_renderer = new CurveRenderer(&r_manager, "crv_shader.vert", "crv_shader.frag");
  Renderer *bgrnd_renderer = new MarchingRenderer("Background shader", "background_raymarching.vert", "background_raymarching.frag");
  Renderer *grnd_renderer = new GroundRenderer("Ground shader", "ground_shader.vert", "ground_shader.frag");
  // Renderer *line = new LineRenderer(&r_manager, "line_shader.vert", "line_shader.frag");
  // Renderer matcap_render(&skeleton, &muscles/*, &character*/);
  //
  r_manager.add_renderer(bgrnd_renderer);
  r_manager.add_renderer(grnd_renderer);
  r_manager.add_renderer(musc_renderer);
  r_manager.add_renderer(skel_renderer);
  // ...

  // Create Objects
  Skeleton skeleton(project, dynamic_cast<MatcapRenderer<Geometry>*>(skel_renderer) /*, Timeline or SimulationManager*/);
  MuscleSystem muscles(project, dynamic_cast<MatcapRenderer<Geometry>*>(musc_renderer), &skeleton);
  //Skin character(project, &skeleton, &muscles);

  // Init the application
  App Simuscle_app(&r_manager, &skeleton, &muscles);
  Simuscle_app.Init();

  // Run the application
  Simuscle_app.Run();

  // clean pointer
  delete skel_renderer;

  return 0;
}

// Ca sera plus compliquer quand j'aurais plusieurs mode, et différente phase au cours de l'application
#endif // !MAIN_CPP
