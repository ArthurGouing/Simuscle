#ifndef GEOMETRY_CPP
#define GEOMETRY_CPP

#include "geometry.h"


#include "Render/renderer.h"
// DebugRenderer::add_line(glm::vec3(0.f,0.f,0.f), glm::vec3(1.f,0.f,1.f), glm::vec3(0.f, 1.f, 0.f));
// DebugRenderer::add_point(vert_pos, glm::vec3(0.2f, 0.2f, 0.6f));
//
using namespace glm;


//******** BASE CLASS :: Geometry ******** 


template <typename Vert, typename Polygone>
Geometry<Vert, Polygone>::Geometry():
  name("empty"), n_verts(0), n_faces(0), offset_id(0), _transformation(mat4(1.f))
{}

template <typename Vert, typename Polygone>
Geometry<Vert, Polygone>::Geometry(std::string init_name):
  name(init_name), n_verts(0), n_faces(0), offset_id(0), _transformation(mat4(1.f))
{}

template <typename Vert, typename Polygone>
Geometry<Vert, Polygone>::Geometry(std::string init_name, std::string file):
  name(init_name), n_verts(0), n_faces(0), offset_id(0), _transformation(mat4(1.f))
{
  create_from_file(file);
}

template <typename Vert, typename Polygone>
void Geometry<Vert, Polygone>::create_from_file(std::string file)
{
  name = file;
  // Declaration des variable
  std::ifstream geom_file(file);
  std::string buff;
  float x, y, z;
  int vert_per_face, id1, id2, id3;
  Vert *v1, *v2, *v3;

  // Open file
  if (!geom_file.is_open())
    Err_Print("Cannot open "+file, "geometry.cpp");
  geom_file >> buff; 
  std::getline(geom_file, buff);
  std::getline(geom_file, buff);
  std::stringstream ssbuff(buff);
  ssbuff >> n_verts;
  ssbuff >> n_faces;

  Info_Print("  Reading " + file + "   (vertices = " + std::to_string(n_verts) + " || faces = " + std::to_string(n_faces) + ")");

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

    Vert vert(id, vert_pos);
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
    std::vector<Vert> vert_face_list;
    ssbuff >> vert_per_face;
    for (int ivert = 0;  < vert_per_face; ++)
    {
      ssbuff >> id;
      Vert* v = &vertex_list[id];
      vert_face_list.push_back(v)
      
    }
    // Create Faces
    Polygone poly(id, vert_face_list);
    face_list.push_back(poly);
    // Add neighboor info // faire une fonction
    for (int ivert = 0; ivert < vert_per_face; ivert++) {
      Vert* v = vert_face_list[id];
      // Add face neighboor info
      v->add_face_neighbor(id);

      // Add vert neighboor info
      for(auto& v2 : vert_face_list) {
        v->add_vert_neighbor(v2)
      }
    }
  }

  // Compute normal geometry
  compute_normals();

  // Cleanning
  geom_file.close();
}

// Geometry::Geometry(std::vector<float> *vertices, std::vector<int> *indices):
//   n_verts(vertices->size()/3), n_faces(indices->size()/3), _transformation(mat4(1.f))
// {
//   float x, y, z;
//   int id1, id2, id3;
//   Vertex *v1, *v2, *v3;
//   Info_Print("nb vertices: "+std::to_string(int(n_verts))+"|size: "+std::to_string(vertices->size()));
//   Info_Print("nb indices: "+std::to_string(int(n_faces))+"|size :"+std::to_string(indices->size()));
//   // Init Vertex array
//   // vert_arr vert_init;
//   // vert_init.pos = vec3(0.0f);
//   // vert_init.normal = vec3(0.0f);
//   // vert_values.resize(n_verts, vert_init); // je devrais init les ptr des vertex_list et face_list ici
//   // Faire pareil avec tous les autres vector<Type> ??? ou juste lui car c'est un tableau de poiteur
//   for (int id = 0; id < n_verts ; id++)
//   {
//     x = vertices->at(3*id + 0);
//     y = vertices->at(3*id + 1);
//     z = vertices->at(3*id + 2);
//     vec3 vert_pos = vec3(x, y, z);
// 
//     Vertex vert(id, vert_pos);
//     vertex_list.push_back(vert);
//   }
// 
//   // Create Faces
//   Info_Print("process faces");
//   for (int id = 0; id < n_faces; id++)
//   {
//     // Find corresponding vertex
//     id1 = indices->at(3*id + 0); 
//     id2 = indices->at(3*id + 1); 
//     id3 = indices->at(3*id + 2); 
//     v1 = &vertex_list[id1];
//     v2 = &vertex_list[id2];
//     v3 = &vertex_list[id3];
//     // Create Faces
//     Triangle tri(id, v1, v2, v3);
//     face_list.push_back(tri);
//     // Add neighboor info // faire une fonction
//     v1->add_face_neighbor(id); v2->add_face_neighbor(id); v3->add_face_neighbor(id);
//     v1->add_vert_neighbor(v2->id); v1->add_vert_neighbor(v3->id);
//     v2->add_vert_neighbor(v1->id); v2->add_vert_neighbor(v3->id);
//     v3->add_vert_neighbor(v1->id); v3->add_vert_neighbor(v2->id);
//   }
//   compute_normals();
// }

template <typename Vert, typename Polygone>
void Geometry<Vert, Polygone>::compute_normals()
{
  for (int i = 0; i < n_faces; i++)
    face_list[i].compute_normal(); // Useless after the reading
 
  for (int i = 0; i < n_verts; i++)
    vertex_list[i].compute_vert_normal(&face_list);

}

template <typename Vert, typename Polygone>
vert_arr Geometry<Vert, Polygone>::compute_value(int id)
{
  vert_arr vert;
  vert.pos = vec3(_transformation * vec4(vertex_list[id].pos, 1.f));
  vert.normal = vec3(_transformation * vec4(vertex_list[id].normal, 1.f));
  return vert;
}

template <typename Vert, typename Polygone>
vec3 Geometry<Vert, Polygone>::ray_intersection(Ray r)
{
  // La collisiont ray_point, itére sur les vertices => bc moins d'element a parcourir car on a moins de points que de face
  float t_min = std::numeric_limits<float>::max();
  vec3 P;
  for(auto& face : face_list) //TODO : better with faces ptr ???
  {
    // Collision point between triangle plane and ray
    float t = (face.distance - dot(r.origin, face.normal) / dot(r.direction, face.normal));
    if (t<0 or t>t_min) continue;
    t_min = t;

    vec3 P_temp  = r.cast(t);
    vec3 v1 = face.get_v1_pos();
    vec3 v2 = face.get_v2_pos();
    vec3 v3 = face.get_v3_pos();
    
    // Test if the point is in the triangle (Same Side technic)
    bool AB_side = dot(cross(v2-v1, P_temp-v1), cross(v2-v1, v3-v1)) >= 0;
    bool BC_side = dot(cross(v3-v2, P_temp-v2), cross(v3-v2, v1-v2)) >= 0;
    bool AC_side = dot(cross(v3-v1, P_temp-v1), cross(v3-v1, v2-v1)) >= 0;

    if (AB_side && BC_side && AC_side) {
      P = P_temp; // TODO: En pratique on pourrait return au 1er P trouver, on ne devrait pas avoir plusieurs collisions possible
    }
  }
  return P;
}

template <typename Vert, typename Polygone>
vec3 Geometry<Vert, Polygone>::ray_intersection_approx(Ray r)
{
  // La collisiont ray_point, itére sur les vertices => bc moins d'element a parcourir car on a moins de points que de face
  float value = std::numeric_limits<float>::max();
  vec3 P;
  for(auto& v : vertex_list) //TODO : better with faces ptr ???
  {
    vec3 intersect = r.origin + dot(v.pos-r.origin, r.direction) * r.direction;
    float test_value = length(v.pos - intersect);
    if (test_value < value){
      value = test_value;
      P = v.pos;
    }
  }
  return P;
}

template <typename Vert, typename Polygone>
Geometry<Vert, Polygone>::~Geometry()
{}


//******** INHERITED CLASS :: Bone ******** 


GeomBone::GeomBone(): Geometry<Vertex, Triangle>() {}

GeomBone::GeomBone(std::string name): Geometry<Vertex, Triangle>(name) {}

GeomBone::GeomBone(std::string name, std::string file): Geometry<Vertex, Triangle>(name, file) {}

glm::vert_arr GeomBone::compute_value (int vert_id)
{
  vert_arr vert;
  vert.pos = vec3(_transformation * vec4(vertex_list[id].pos, 1.f));
  vert.normal = vec3(_transformation * vec4(vertex_list[id].normal, 1.f));
  return vert;
}

GeomBone::~GeomBone() {}


//******** INHERITED CLASS :: Bone ******** 


GeomMusc::GeomMusc(): Geometry<Vertex, Triangle>() {}

GeomMusc::GeomMusc(std::string name): Geometry<Vertex, Triangle>(name) {}

GeomMusc::GeomMusc(std::string name, std::string file, Curve* crv): Geometry<Vertex, Triangle>(name, file), _crv(crv) {}

glm::vert_arr GeomMusc::compute_value (int vert_id)
{
  vert_arr vert;
  Vertex v = vertex_list[id];
  vert.pos = _deformation->apply_rotation(v.pos, _curve->compute_curve_point(v.t), v.t) + _deformation.at(v.t); // Inclure la transformation
  vert.normal = v.normal;
}

GeomMusc::~GeomMusc() {}


//******** INHERITED CLASS :: Bone ******** 


GeomSkin::GeomSkin(): Geometry<SkinVertex, Quad>() {}

GeomSkin::GeomSkin(std::string name): Geometry<SkinVertex, Quad>(std::string name) {}

GeomSkin::GeomSkin(std::string name, std::string file): Geometry<SkinVertex, Quad>(std::string name, std::string file) {}

glm::vert_arr GeomSkin::compute_value(int vert_id)
{
  vert_arr vert;
  SkinVertex v = vertex_list[id];
  vert.pos = v.pos;
  vert.normal = v.normal;
}

GeomSkin::~GeomSkin() {}


#endif // !GEOMETRY_CPP
