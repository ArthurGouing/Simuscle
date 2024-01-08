#ifndef ELEMENT_CPP
#define ELEMENT_CPP

//******** LIBRARY ******** 
#include "element.h"

using namespace glm;

Vertex::Vertex(int init_id, vec3 init_pos):
  id(init_id), pos(init_pos), normal(0.f), n_face_neighbor(0), n_vert_neighbor(0)
{}

Vertex::Vertex(Vertex* vert):
  id(vert->id), pos(vert->pos), normal(vert->normal),
  n_face_neighbor(vert->n_face_neighbor), n_vert_neighbor(vert->n_face_neighbor)
{}

void Vertex::compute_vert_normal(std::vector<Triangle> *faces)
{
  for (int i = 0; i < n_face_neighbor; i++)
  {
    Vertex *v2, *v3;
    faces->at(_face_neighbor[i]).get_vert_neighbor(this, &v2, &v3);
    float a = angle(normalize(v2->pos-pos), normalize(v3->pos-pos));
    normal += a * faces->at(_face_neighbor[i]).normal;
  }
  normal = normalize(normal);
}


void Vertex::add_face_neighbor(int id_face)
{
  for (int i = 0; i < n_face_neighbor; i++) {
    if (_face_neighbor[i] == id_face) {
      // Err_Print("This face neighbor ("+std::to_string(id_face)+") is already known", "element.cpp");
      return;
    }
  }
  _face_neighbor.push_back(id_face);
  n_face_neighbor += 1;
  return;
}
void Vertex::add_vert_neighbor(int id_vert)
{
  for (int i = 0; i < n_vert_neighbor; i++) {
    if (_vert_neighbor[i] == id_vert) {
      // Err_Print("This vert neighbor ("+std::to_string(id_vert)+") is already known", "element.cpp");
      return;
    }
  }
  _vert_neighbor.push_back(id_vert);
  n_vert_neighbor += 1;
  return;
}

Triangle::Triangle(int id, Vertex *v1, Vertex *v2, Vertex *v3):
  _id(id), _v1(v1), _v2(v2), _v3(v3)
{
  compute_normal();
}
Triangle::~Triangle()
{
  // TODO
}

void Triangle::compute_normal()
{
  normal =  normalize(cross(_v2->pos-_v1->pos, _v3->pos-_v1->pos));
}

void Triangle::get_vert_neighbor(Vertex *v1, Vertex **v2, Vertex **v3)
{
  if (v1==_v1) {
    *v2 = _v2;
    *v3 = _v3;
  }
  else if (v1==_v2) {
    *v2 = _v3;
    *v3 = _v1;
  } else if (v1==_v3) {
    *v2 = _v1;
    *v3 = _v2;
  } else {
    Err_Print("v1 is not in this face", "element.cpp");
  }
  
}
#endif // !ELEMENT_CPP
