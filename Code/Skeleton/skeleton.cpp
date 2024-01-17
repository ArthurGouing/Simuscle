#ifndef SKELETON_CPP
#define SKELETON_CPP

#include "skeleton.h"
// Constructors
Skeleton::Skeleton()
{}

Skeleton::Skeleton(std::string project, MatcapRenderer<Geometry>* init_renderer):
  renderer(init_renderer), reset_pose(false)
{
  Title_Print("Init Skeleton");

  /******** Read BVH ********/
  std::string bvh_file = project+"animation.bvh";
  Info_Print("Reading BVH file : " + bvh_file);
  Root_Bone.create_from_file(bvh_file);
  _nb_frames = Root_Bone._nb_frames;
  // Root_Bone.print_bone();

  /******** Read Bones parameters ********/
  std::string bone_file = project+"Bones/bones_info.json";
  Info_Print("Reading Bone_info file : " + bone_file);
  BonesInfo bonesinfo(bone_file);
  // bonesinfo.print();

  /******** Load bone meshes ********/
  Info_Print("Loading bones geometries: ");
  int indice_offset = 0;
  Root_Bone.create_geometry(bonesinfo, project, &indice_offset);

  Info_Print("Link geometries to renderer ("+renderer->name+"). ");
  Root_Bone.link_geometry(renderer);

  Info_Print("Done");
}

void Skeleton::compute(int frame)
{
  // Compute new values at the frame 
  Root_Bone.compute_transform(frame);
  renderer->update_VBO();
}

void Skeleton::draw_skeleton_stick()
  /* Draw all the bones of the skeleton in the OpenGL pipline.
   * mode wire i.e. draw skeleton as a set of OpenGL_Line
   */
{
  Info_Print("Stick case");
  // Update bone position to the VBO
  // TODO:
  // ...
  // ...

  // Draw the skeleton
  //glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, n_values/*TODO*/, GL_UNSIGNED_INT, 0);
}

void Skeleton::draw_skeleton_mesh(bool wire_mode)
{
  /* Draw all the bones of the skeleton in the OpenGL pipline.
   * mode mesh i.e. draw the geometry of each bone of the skeleton
   */

   // draw our first triangle
  glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, n_values, GL_UNSIGNED_INT, 0);
  // glDrawElements(GL_TRIANGLES, n_values, GL_UNSIGNED_INT, 0);

  // Draw the skeleton
  // glBindVertexArray(VAO);
  // if (wire_mode) {
  //   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // } else {
  //   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // }
}

Bone* Skeleton::find_bone(std::string bone_name)
{
  Bone* bone = nullptr;
  bone = Root_Bone.find_bone(bone_name);
  if (bone==nullptr) {
    Root_Bone.print_bone();
    Err_Print("The bone '" + bone_name + "' doesn't belong to the skelton.", "skeleton.cpp");
  }
  return bone;
}

void Skeleton::UI_pannel()
{
  ImGui::Begin("Skeleton parameters");
  ImGui::Checkbox("Reset pose", &reset_pose);
  ImGui::End();
}

Skeleton::~Skeleton()
{
  delete renderer;
}

#endif // !SKELETON_CPP
