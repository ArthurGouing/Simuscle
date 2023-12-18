#ifndef GEOMETRY_CPP
#define GEOMETRY_CPP

#include "geometry.h"

using namespace glm;

Geometry::Geometry():
  n_verts(0), n_faces(0)
{}

Geometry::Geometry(std::string file):
  n_verts(0), n_faces(0)
{
  create_from_file(file);
}

void Geometry::create_from_file(std::string file)
{
  // Init
  std::ifstream geom_file(file);
  std::string buff;
  float x, y, z;
  int vert_on_face, id1, id2, id3;
  Vertex *v1, *v2, *v3;
  // Open file
  if (!geom_file.is_open())
    Err_Print("Cannot open "+file, "geometry.cpp");
  geom_file >> buff; 
  std::getline(geom_file, buff);
  std::getline(geom_file, buff);
  std::stringstream ssbuff(buff);
  ssbuff >> n_verts;
  ssbuff >> n_faces;

  Info_Print("Reading " + file);
  Info_Print("  Number of vertices = " + std::to_string(n_verts));
  Info_Print("  Number of faces = " + std::to_string(n_faces));

  // Init vert_values
  vert_arr vert_init;
  vert_init.pos = vec3(0.0f);
  vert_init.normal = vec3(0.0f);
  vert_values.resize(n_verts, vert_init); // je devrais init les ptr des vertex_list et face_list ici
 
  // Process vertex
  for (int id = 0; id < n_verts; id++)
  {
    // Title_Print(std::to_string(id));
      if (!std::getline(geom_file, buff)) {
        Err_Print("The file suddently ended while processing vertex", "geometry.cpp");
        exit(0);
      }
    std::stringstream ssbuff(buff); 
    ssbuff >> x; ssbuff >> y; ssbuff >> z;
    vec3 vert_pos = vec3(x, y, z);
    vert_values[id].pos = vert_pos;

    Vertex vert(id, &vert_values);
    vertex_list.push_back(vert);
  }
 
  // Process Faces
  for (int id = 0; id < n_faces; id++) 
  {
    // Info_Print("face processing : "+std::to_string(id));
    if (!std::getline(geom_file, buff)) {
      Err_Print("Can't get line ; the file probably ended while processing faces", "geometry.cpp");
      break;
    }
    std::stringstream ssbuff(buff); 
    ssbuff >> vert_on_face; ssbuff >> id1; ssbuff >> id2; ssbuff >> id3;
    if (vert_on_face!=3) {
      Err_Print("One of the face is not a triangle ; currently not supported", "geometry.cpp");
      break;
    }
    v1 = &vertex_list[id1];
    v2 = &vertex_list[id2];
    v3 = &vertex_list[id3];
    // Create Faces
    Triangle tri(id, v1, v2, v3);
    face_list.push_back(tri);
    // Create array for buffer
    face_indices.push_back(id1); 
    face_indices.push_back(id2); 
    face_indices.push_back(id3); 
    // Add neighboor info // faire une fonction
    v1->add_face_neighbor(id); v2->add_face_neighbor(id); v3->add_face_neighbor(id);
    v1->add_vert_neighbor(v2->_id); v1->add_vert_neighbor(v3->_id);
    v2->add_vert_neighbor(v1->_id); v2->add_vert_neighbor(v3->_id);
    v3->add_vert_neighbor(v1->_id); v3->add_vert_neighbor(v2->_id);
  }
  compute_normals();
  // Info_Print("End Read faces info");
  // std::cout << "nb values in face_indies : " << face_indices.size() << std::endl;
  // std::cout << "nb faces                 : " << face_indices.size()/3 << std::endl;
  // std::cout << "nb faces                 : " << n_faces << std::endl;
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
  // Init Vertex array
  vert_arr vert_init;
  vert_init.pos = vec3(0.0f);
  vert_init.normal = vec3(0.0f);
  vert_values.resize(n_verts, vert_init); // je devrais init les ptr des vertex_list et face_list ici
  // Faire pareil avec tous les autres vector<Type> ??? ou juste lui car c'est un tableau de poiteur
  for (int id = 0; id < n_verts ; id++)
  {
    x = vertices->at(3*id + 0);
    y = vertices->at(3*id + 1);
    z = vertices->at(3*id + 2);
    vec3 vert_pos = vec3(x, y, z);
    vert_values[id].pos = vert_pos;

    Vertex vert(id, &vert_values);
    vertex_list.push_back(vert);
  }

  // Create Faces
  Info_Print("process faces");
  for (int id = 0; id < n_faces; id++)
  {
    // Find corresponding vertex
    id1 = indices->at(3*id + 0); 
    id2 = indices->at(3*id + 1); 
    id3 = indices->at(3*id + 2); 
    v1 = &vertex_list[id1];
    v2 = &vertex_list[id2];
    v3 = &vertex_list[id3];
    // Create Faces
    Triangle tri(id, v1, v2, v3);
    face_list.push_back(tri);
    // Create array for buffer
    face_indices.push_back(id1);
    face_indices.push_back(id2);
    face_indices.push_back(id3);
    // Add neighboor info // faire une fonction
    v1->add_face_neighbor(id); v2->add_face_neighbor(id); v3->add_face_neighbor(id);
    v1->add_vert_neighbor(v2->_id); v1->add_vert_neighbor(v3->_id);
    v2->add_vert_neighbor(v1->_id); v2->add_vert_neighbor(v3->_id);
    v3->add_vert_neighbor(v1->_id); v3->add_vert_neighbor(v2->_id);
  }
  compute_normals();
}

Geometry::~Geometry()
{
  // TODO
}

void Geometry::compute_normals()
// Ca va pas du tout...
{
  for (int i = 0; i < n_faces; i++)
    face_list[i].compute_normal();
 
  for (int i = 0; i < n_verts; i++)
    vertex_list[i].compute_vert_normal(&face_list);

}

void Geometry::set_Buffers()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vert_values.size() * 6 * sizeof(float), &vert_values[0], GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_indices.size() * sizeof(float), face_indices.data(), GL_STATIC_DRAW); //TODO: use dynamic draw

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}
#endif // !GEOMETRY_CPP
