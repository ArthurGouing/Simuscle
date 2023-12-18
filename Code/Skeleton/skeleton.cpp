#ifndef SKELETON_CPP
#define SKELETON_CPP

#include "skeleton.h"
// Constructors
Skeleton::Skeleton()
{}

Skeleton::Skeleton(std::string project):
  reset_pose(false)
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

  // Init indices array
  Root_Bone.set_indices(&indices);

  // Init values array
  int values_size=0;
  Root_Bone.get_values_size(&values_size);
  values.resize(values_size); // values need to be init before computing his values
  n_values = 6 * values_size + 10000;
  // n_values = 6 * values_size; // TODO: erreur, the n_values is not correct. I have no clues why that...

  // Set VBO and sent it to the GPU
  // Root_Bone.update_values(&values, 0, reset_pose); // useless already in update_buffers
  update_buffers(0);
}

void Skeleton::update_buffers(int frame)
{
  // Compute new values at the frame 
  Root_Bone.compute_transform(frame);
  Root_Bone.update_values(&values, frame, reset_pose);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

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
}

#endif // !SKELETON_CPP
