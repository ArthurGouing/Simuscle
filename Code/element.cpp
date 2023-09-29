#ifndef ELEMENT_CPP
#define ELEMENT_CPP

//******** LIBRARY ******** 
#include "element.h"
#include "tools.h"

using namespace glm;

Vertex::Vertex(int id, std::vector<vert_arr> *vert_values):
  _id(id), _pos(&vert_values->at(id).pos), _normal(&vert_values->at(id).normal)
{
  Info_Print("Init vertex");
  std::cout << "  " << _pos << std::endl;
  // To cast to a float ptr, so we can access x, y and z
  // float *ptr = (float *)_pos;
}
Vertex::Vertex(Vertex* vert):
  _id(vert->_id), _pos(vert->_pos), n_face_neighbor(0), _normal(vert->_normal)
{
  Info_Print("Constructeur par poiteur");
  //TODO:
}

void Vertex::compute_vert_normal(std::vector<Triangle> *faces)
{
  Info_Print("Compute Normal");
  Info_Print(std::to_string(n_face_neighbor)+ " face _face_neighbor");
  for (int i = 0; i < n_face_neighbor; i++)
  {
    *_normal += faces->at(_face_neighbor[i]).get_normal();
    // _normal += dot(v2-v1,v3-v1) * faces->at(i).get_normal();
  }
  *_normal = normalize(*_normal);
  Vec3_Print("normal", *_normal);
}



Triangle::Triangle(int id, Vertex *v1, Vertex *v2, Vertex *v3):
  _id(id), _v1(v1), _v2(v2), _v3(v3)
{
  // Info_Print(std::to_string(_v1->_id));
  _normal =  normalize(cross(_v2->get_pos()-_v1->get_pos(), _v3->get_pos()-_v1->get_pos()));
  // Vec3_Print("normal vector", _normal);
}
Triangle::~Triangle()
{
  // TODO
}
#endif // !ELEMENT_CPP
