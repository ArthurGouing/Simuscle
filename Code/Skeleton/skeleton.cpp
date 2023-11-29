#ifndef SKELETON_CPP
#define SKELETON_CPP

#include "skeleton.h"
// Constructors
Skeleton::Skeleton()
{}

Skeleton::Skeleton(std::string project)
{
  Title_Print("Init Skeleton");

  /******** Read BVH ********/
  std::string bvh_file = project+"animation.bvh";
  Info_Print("Reading BVH file : " + bvh_file);
  Root_Bone.create_from_file(bvh_file);
  // Root_Bone.print_bone();

  /******** Read Bones parameters ********/
  std::string bone_file = project+"Bones/bones_info.json";
  Info_Print("Reading Bone_info file : " + bone_file);
  BonesInfo bonesinfo(bone_file);
  // bonesinfo.print();

  /******** Load bone meshes ********/
  Info_Print("Loading bones meshes");
  int indice_offset = 0;
  Root_Bone.create_geometry(bonesinfo, project, &indice_offset);

  Info_Print("Done");
}

void Skeleton::init_buffers()
{
  // Create VBO
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

  n_values = 6 * values.size();

  Info_Print("Done");
  values.clear();
  indices.clear();
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
  return bone;
}


Skeleton::~Skeleton()
{}

#endif // !SKELETON_CPP
