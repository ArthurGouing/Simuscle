#ifndef GEOMETRY_CPP
#define GEOMETRY_CPP

#include "geometry.h"

using namespace glm;

Geometry::Geometry():
  name("empty"), n_verts(0), n_faces(0), offset_id(0), _transformation(mat4(1.f))
{}

Geometry::Geometry(std::string file):
  name(file), n_verts(0), n_faces(0), offset_id(0), _transformation(mat4(1.f))
{
  create_from_file(file);
}

void Geometry::create_from_file(std::string file)
{
  name = file;
  std::cout << "  " << file << std::endl;
  // Declaration des variable
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

  // Process vertex
  for (int id = 0; id < n_verts; id++)
  {
    if (!std::getline(geom_file, buff)) {
      Err_Print("The file suddently ended while processing vertex", "geometry.cpp");
      exit(0);
    }
    std::stringstream ssbuff(buff); 
    ssbuff >> x; ssbuff >> y; ssbuff >> z;
    vec3 vert_pos = vec3(x, y, z);
    // vert_values[id].pos = vert_pos;

    Vertex vert(id, vert_pos);
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
    // Add neighboor info // faire une fonction
    v1->add_face_neighbor(id); v2->add_face_neighbor(id); v3->add_face_neighbor(id);
    v1->add_vert_neighbor(v2->id); v1->add_vert_neighbor(v3->id);
    v2->add_vert_neighbor(v1->id); v2->add_vert_neighbor(v3->id);
    v3->add_vert_neighbor(v1->id); v3->add_vert_neighbor(v2->id);
  }
  compute_normals();

  // Cleanning
  geom_file.close();
  // Info_Print("End Read faces info");
  // std::cout << "nb values in face_indies : " << face_indices.size() << std::endl;
  // std::cout << "nb faces                 : " << face_indices.size()/3 << std::endl;
  // std::cout << "nb faces                 : " << n_faces << std::endl;
}

Geometry::Geometry(std::vector<float> *vertices, std::vector<int> *indices):
  n_verts(vertices->size()/3), n_faces(indices->size()/3), _transformation(mat4(1.f))
{
  float x, y, z;
  int id1, id2, id3;
  Vertex *v1, *v2, *v3;
  Info_Print("nb vertices: "+std::to_string(int(n_verts))+"|size: "+std::to_string(vertices->size()));
  Info_Print("nb indices: "+std::to_string(int(n_faces))+"|size :"+std::to_string(indices->size()));
  // Init Vertex array
  // vert_arr vert_init;
  // vert_init.pos = vec3(0.0f);
  // vert_init.normal = vec3(0.0f);
  // vert_values.resize(n_verts, vert_init); // je devrais init les ptr des vertex_list et face_list ici
  // Faire pareil avec tous les autres vector<Type> ??? ou juste lui car c'est un tableau de poiteur
  for (int id = 0; id < n_verts ; id++)
  {
    x = vertices->at(3*id + 0);
    y = vertices->at(3*id + 1);
    z = vertices->at(3*id + 2);
    vec3 vert_pos = vec3(x, y, z);

    Vertex vert(id, vert_pos);
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
    // Add neighboor info // faire une fonction
    v1->add_face_neighbor(id); v2->add_face_neighbor(id); v3->add_face_neighbor(id);
    v1->add_vert_neighbor(v2->id); v1->add_vert_neighbor(v3->id);
    v2->add_vert_neighbor(v1->id); v2->add_vert_neighbor(v3->id);
    v3->add_vert_neighbor(v1->id); v3->add_vert_neighbor(v2->id);
  }
  compute_normals();
}

void Geometry::compute_normals()
// Ca va pas du tout...
{
  for (int i = 0; i < n_faces; i++)
    face_list[i].compute_normal();
 
  for (int i = 0; i < n_verts; i++)
    vertex_list[i].compute_vert_normal(&face_list);

}

vert_arr Geometry::compute_value(int id)
{
  vert_arr vert;
  vert.pos = vec3(_transformation * vec4(vertex_list[id].pos, 1.f));
  vert.normal = vec3(_transformation * vec4(vertex_list[id].normal, 1.f));
  return vert;
}

Geometry::~Geometry()
{}

#endif // !GEOMETRY_CPP
