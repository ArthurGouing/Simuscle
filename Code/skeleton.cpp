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
  std::string bvh_file = project+"animation.bvh";
  std::string bone_file = project+"Bones/bones_info.json";
  Root_Bone.create_from_file(bvh_file);
  Info_Print("Done Read BVH");
  Info_Print("Start read bone file");
  BonesInfo bonesinfo(bone_file);
  bonesinfo.print();
  int indice_offset = 0;
  Root_Bone.create_geometry(bonesinfo, project, &indice_offset);
  Info_Print("Done read bones");
}

void Skeleton::draw_skeleton_stick()
{
  /* Draw all the bones of the skeleton in the OpenGL pipline.
   * mode wire i.e. draw skeleton as a set of OpenGL_Line
   */
  Info_Print("Stick case");
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
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  // Update bonne position to the VBO
 

  std::vector<glm::vert_arr> values;
  std::vector<int>           indices;


  Root_Bone.update_values(&values, &indices);

 
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, values.size() * 6 * sizeof(float), &(values[0]), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

   // draw our first triangle
  glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, values.size() * 6, GL_UNSIGNED_INT, 0);

  // Draw the skeleton
  // glBindVertexArray(VAO);
  // if (wire_mode) {
  //   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // } else {
  //   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  // }

  // glDrawElements(GL_TRIANGLES, 3 * n_faces, GL_UNSIGNED_INT, 0);
}

Skeleton::~Skeleton()
{}

#endif // !SKELETON_CPP
