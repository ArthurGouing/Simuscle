#ifndef ELEMENT_CPP
#define ELEMENT_CPP

//******** LIBRARY ******** 
#include "element.h"

using namespace glm;

Vertex::Vertex(int id, std::vector<vert_arr> *vert_values):
  _id(id), _pos(&vert_values->at(id).pos), _normal(&vert_values->at(id).normal),
  n_face_neighbor(0), n_vert_neighbor(0)
{
}
Vertex::Vertex(Vertex* vert):
  _id(vert->_id), _pos(vert->_pos), _normal(vert->_normal),
  n_face_neighbor(vert->n_face_neighbor), n_vert_neighbor(vert->n_face_neighbor)
{
}

void Vertex::compute_vert_normal(std::vector<Triangle> *faces)
{
  for (int i = 0; i < n_face_neighbor; i++)
  {
    Vertex *v2, *v3;
    faces->at(_face_neighbor[i]).get_vert_neighbor(this, &v2, &v3);
    float a = angle(normalize(v2->get_pos()-get_pos()), normalize(v3->get_pos()-get_pos()));
    *_normal += a * faces->at(_face_neighbor[i]).get_normal();
  }
  *_normal = normalize(*_normal);
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
  _normal =  normalize(cross(_v2->get_pos()-_v1->get_pos(), _v3->get_pos()-_v1->get_pos()));
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
