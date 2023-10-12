#ifndef SKELETON_CPP
#define SKELETON_CPP

#include "skeleton.h"

// Constructors
Skeleton::Skeleton()
{}

Skeleton::Skeleton(std::string project)
{
  Title_Print("Init Skeleton");
  // Find bvh file
  std::string bvh_file = project+"Human_anim.bvh";
  Root_Bone.create_from_file(bvh_file);
  Info_Print("Done Read BVH");
}

void Skeleton::draw_skeleton_wire()
{
  /* Draw all the bones of the skeleton in the OpenGL pipline.
   * mode wire i.e. draw skeleton as a set of OpenGL_Line
   */
  Info_Print("Wire case");
  // Update bone position to the VBO
  // TODO:
  // ...
  // ...

  // Draw the skeleton
  //glUseProgram(shaderProgram);
  glBindVertexArray(VAO);
  glDrawElements(GL_LINES, n_faces/*TODO*/, GL_UNSIGNED_INT, 0);
}

void Skeleton::draw_skeleton_mesh(bool wire_mode)
{
  /* Draw all the bones of the skeleton in the OpenGL pipline.
   * mode mesh i.e. draw the geometry of each bone of the skeleton
   */
  Info_Print("Mesh case");
  // Update bonne position to the VBO
  // TODO:
  // ...
  // ...

  // Draw the skeleton
  glBindVertexArray(VAO);
  if (wire_mode) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  glDrawElements(GL_TRIANGLES, 3 * n_faces, GL_UNSIGNED_INT, 0);
}

Skeleton::~Skeleton()
{}

#endif // !SKELETON_CPP
