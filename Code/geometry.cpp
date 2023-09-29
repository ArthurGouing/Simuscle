#ifndef GEOMETRY_CPP
#define GEOMETRY_CPP

#include "geometry.h"

using namespace glm;

Geometry::Geometry(std::string file)
{
//   // Open file
//   std::ifstream geom_file(file);
//   std::string buff;
//   float x, y, z;
//   int vert_on_face, id1, id2, id3;
//   Vertex *v1, *v2, *v3;
//   // Init
//   if (!geom_file.is_open())
//     Err_Print("Cannont open "+file, "geometry.cpp");
//   geom_file >> buff; std::cout << "Read geometry from " << buff <<" format" << std::endl;
//   std::getline(geom_file, buff);
//   std::getline(geom_file, buff);
//   std::cout << buff << std::endl;
//   std::stringstream ssbuff(buff);
//   ssbuff >> n_verts;
//   std::cout << "Number of vertices = " << n_verts << std::endl;
//   ssbuff >> n_faces;
//   std::cout << "Number of faces    = " << n_faces << std::endl;
//   Info_Print(std::to_string(n_faces));
// 
//   // Process vertex
//   for (int id = 0; id < n_verts; id++)
//   {
//     if (!std::getline(geom_file, buff)) {
//       Err_Print("The file suddently ended while processing vertex", "geometry.cpp");
//       break;
//   }
//     std::stringstream ssbuff(buff); 
//     ssbuff >> x; ssbuff >> y; ssbuff >> z;
//     Vertex vert(id, x, y, z);
// 
//     vertex_list.push_back(vert);
//     vert_pos.push_back(x);
//     vert_pos.push_back(y);
//     vert_pos.push_back(z);
//   }
//   // geom_file >> buff; std::cout << buff << std::endl;
//   std::cout << "nb values in vert_pos : " << vert_pos.size() << std::endl;
//   std::cout << "nb vert               : " << vert_pos.size()/3 << std::endl;
// 
//   // Process Faces
//   for (int id = 0; id < n_faces; id++) 
//   {
//     // Info_Print("face processing : "+std::to_string(id));
//     if (!std::getline(geom_file, buff)) {
//       Err_Print("Can't get line ; the file probably ended while processing faces", "geometry.cpp");
//       break;
//     }
//     std::stringstream ssbuff(buff); 
//     ssbuff >> vert_on_face; ssbuff >> id1; ssbuff >> id2; ssbuff >> id3;
//     if (vert_on_face!=3) {
//       Err_Print("One of the face is not a triangle ; currently not supported", "geometry.cpp");
//       break;
//     }
//     v1 = &vertex_list[id1];
//     v2 = &vertex_list[id2];
//     v3 = &vertex_list[id3];
//     Triangle tri(id, v1, v2, v3);
// 
//     // Create Faces
//     // Info_Print(std::to_string(v1->_id));
// 
//     face_list.push_back(tri);
//     face_indices.push_back(id1); 
//     face_indices.push_back(id2); 
//     face_indices.push_back(id3); 
// 
//     // Compute vertex normal
//     v1->compute_vert_normal(id, tri.get_normal());
//     v2->compute_vert_normal(id, tri.get_normal());
//     v3->compute_vert_normal(id, tri.get_normal());
//   }
//   Info_Print("End Read faces info");
//   std::cout << "nb values in face_indies : " << face_indices.size() << std::endl;
//   std::cout << "nb faces                 : " << face_indices.size()/3 << std::endl;
//   std::cout << "nb faces                 : " << n_faces << std::endl;
// 
  Info_Print("Void");
}

Geometry::Geometry(std::vector<float> *vertices, std::vector<int> *indices):
  n_verts(vertices->size()/3), n_faces(indices->size()/3)
  // vert_pos(*vertices), face_indices(*indices)
{
  float x, y, z;
  int id1, id2, id3;
  Vertex *v1, *v2, *v3;
  Info_Print("nb vertices: "+std::to_string(int(n_verts))+"|size: "+std::to_string(vertices->size()));
  Info_Print("nb indices: "+std::to_string(int(n_faces))+"|size :"+std::to_string(indices->size()));
  //
  // Create Vertex
  vert_arr vert_init;
  vert_init.pos = vec3(0.0f);
  vert_init.normal = vec3(0.0f);
  vert_values.resize(n_verts, vert_init); // je devrais init les ptr des vertex_list et face_list ici
  // Faire pareil avec tous les autres vector<Type> ??? ou juste lui car c'est un tableau de poiteur
  for (int id = 0; id < n_verts ; id++)
  {
    Info_Print("");
    Title_Print(std::to_string(id));
    x = vertices->at(3*id + 0);
    y = vertices->at(3*id + 1);
    z = vertices->at(3*id + 2);
    // Info_Print(std::to_string(id)+", "+std::to_string(x)+", "+std::to_string(y)+", "+std::to_string(z));
    vec3 vert_pos = vec3(x, y, z);
    vert_values[id].pos = vert_pos;
    // std::cout << "  " << &vert_pos << std::endl;
    // std::cout << "  " << &vert_values[id] << std::endl;

    Vertex vert(id, &vert_values);
    // std::cout << "  " << id << "  " << &vert_values.at(id) << std::endl;
    // std::cout << vert._pos << std::endl;
    // Info_Print(std::to_string(id)+", "+std::to_string(vert._pos->x)+", "+std::to_string(vert._pos->y)+", "+std::to_string(vert._pos->z));
    vertex_list.push_back(vert);
    // std::cout << vert._pos << std::endl;
    // std::cout << vertex_list.at(id)._pos << std::endl;
  }

  // Create Faces
  Info_Print("process faces");
  for (int id = 0; id < n_faces; id++)
  {
    // Vertex vert( &vertex_list[face_indices[3*id + 0]]);
    // v1 = &vert;
    id1 = indices->at(3*id + 0); 
    id2 = indices->at(3*id + 1); 
    id3 = indices->at(3*id + 2); 
    Info_Print("Create v1");
    v1 = &vertex_list[id1];
    v2 = &vertex_list[id2];
    v3 = &vertex_list[id3];
    // Create Faces
    Info_Print(std::to_string(v1->_id)+", "+std::to_string(v2->_id)+", "+std::to_string(v3->_id));
    Triangle tri(id, v1, v2, v3);
    face_list.push_back(tri);
    // Create array for buffer
    face_indices.push_back(id1);
    face_indices.push_back(id2);
    face_indices.push_back(id3);
    // Add neighboor info // faire une fonction
    v1->_face_neighbor.push_back(id);
    v2->_face_neighbor.push_back(id);
    v3->_face_neighbor.push_back(id);
    v1->n_face_neighbor += 1;
    v2->n_face_neighbor += 1;
    v3->n_face_neighbor += 1;
    // v1._vert_neighbor.push_back(v2->_id); // il faut voir si il existe déjà avant de le pushback 
    // v1._vert_neighbor.push_back(v3->_id);
  }
  compute_vert_normal();
}

Geometry::~Geometry()
{
  // TODO
}

void Geometry::compute_vert_normal()
// Ca va pas du tout...
{
  for (int i = 0; i < n_verts; i++)
  {
    vertex_list[i].compute_vert_normal(&face_list);
    vert_values[i].normal = *vertex_list[i]._normal;
    vertex_list[i]._normal = &vert_values[i].normal;

  }
}

void Geometry::set_Buffers()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  Info_Print("Load VBO");
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vert_values.size() * 6 * sizeof(float), &vert_values[0], GL_STATIC_DRAW);
  Info_Print("Load EBO");
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_indices.size() * sizeof(float), face_indices.data(), GL_STATIC_DRAW);

  Title_Print("Test vert_value");
  for (int i = 0; i < 6; i++) {
    std::cout << *((float *)(&vert_values[0])+i) << " ";
    // std::cout << "  v"+std::to_string(i)+" = " << *(float *)&vert_values[i] << " "<< *((float *)(&vert_value[i])+1) << " "<< *((float *)(&vert_value[i])+2) << " " << std::endl;)
    // std::cout <<  "  v"+std::to_string(i)+" = " << (float *)&vert_values[i] << std::endl;
  }
  std::cout << std::endl;

  Info_Print("Attribute pointer");
  // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
  //                   0 ou 1 pour le layout dans le shader
  glEnableVertexAttribArray(1);
}
#endif // !GEOMETRY_CPP
