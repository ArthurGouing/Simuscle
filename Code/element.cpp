#ifndef ELEMENT_CPP
#define ELEMENT_CPP

//******** LIBRARY ******** 
#include "element.h"
#include "tools.h"

using namespace glm;

Vertex::Vertex(int id, std::vector<vec3> *vert_values):
  _id(id), _pos(&vert_values->at(id))
{
  std::cout << "  " << _pos << std::endl;
  // To cast to a float ptr, so we can access x, y and z
  // float *ptr = (float *)_pos;
}
Vertex::Vertex(Vertex* vert):
  _id(vert->_id), _pos(vert->_pos), n_face_neighbor(0), _normal(vec3(0.0f))
{
  Info_Print("Constructeur par poiteur");
  //TODO:
}
void Vertex::compute_vert_normal(int face_id, vec3 face_normal)
{
  // face id useless
  _normal = float(n_face_neighbor+1) * _normal + face_normal/float(n_face_neighbor+1);
  n_face_neighbor += 1;
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
