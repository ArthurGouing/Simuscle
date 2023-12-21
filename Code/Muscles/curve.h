//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: curve.cpp
// Parent: muscle.cpp
// Date: 30/11/23
// Content: Class that contain all the function for the curve representing the middle section of the muscle 
//**********************************************************************************//
#ifndef CURVE_H
#define CURVE_H

//******** LIBRARY ******** 
#include <cmath>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Eigen_src/Sparse"
#include "material_property.h"
#include "Geometry/element.h"
#include "Tools/tools.h"

struct Qpoint {  // Peut changer, dépend de commment on utilise pos et rot pour déformer le muscle
  Qpoint(){};
  Qpoint(glm::vec3 init_pos, glm::vec3 init_rot): pos(init_pos), rot(init_rot) {};
  glm::vec3 pos;
  glm::vec3 rot;
};

class Deformations
{
  public:
    Deformations(){};
    Deformations(int size, Qpoint v0):deform(size, v0){};

    void resize(int s){deform.resize(s);};
    Qpoint at(int i){return deform.at(i);};
    void update_deform(Eigen::SparseVector<float> x, Eigen::SparseVector<float> x_p){
      Qpoint p0;
      p0.pos.x = x_p.coeffRef(0); p0.pos.y = x_p.coeffRef(1); p0.pos.z = x_p.coeffRef(2);
      p0.rot.x = x_p.coeffRef(3); p0.rot.y = x_p.coeffRef(4); p0.rot.z = x_p.coeffRef(5);
      deform[0] = p0;
      for (int i = 0; i < x.size()/6; i++) {
        Qpoint p;
        p.pos.x = x.coeffRef(6*i + 0); p.pos.y = x.coeffRef(6*i + 1); p.pos.z = x.coeffRef(6*i + 2);
        p.rot.x = x.coeffRef(6*i + 3); p.rot.y = x.coeffRef(6*i + 4); p.rot.z = x.coeffRef(6*i + 5);
        deform[i+1] = p; // Note Eigen ne recommande pas d'utiliser les coeffRef, il faudra chercher une méthode plus opti
      }
      Qpoint pnp1;
      pnp1.pos.x = x_p.coeffRef(6); pnp1.pos.y = x_p.coeffRef(7); pnp1.pos.z = x_p.coeffRef(8);
      pnp1.rot.x = x_p.coeffRef(9); pnp1.rot.y = x_p.coeffRef(10); pnp1.rot.z = x_p.coeffRef(11);
      deform[deform.size()-1] = pnp1;

    };
    void print(){
      for (int i = 0; i < deform.size(); i++) {
        std::cout << "p"<<i<<" pos: "<<deform[i].pos.x <<" "<<deform[i].pos.y <<" "<<deform[i].pos.z<<"  |  ";
        std::cout << "rot: "<<deform[i].rot.x<<" "<<deform[i].rot.y<<" "<<deform[i].rot.z<<" "<< std::endl;
        }
    }

  private:
    std::vector<Qpoint> deform;
};

// typedef std::vector<Qpoint> Deformations;

class Curve
{
  public:
    Curve(){};
    Curve(const Curve&) = default;
    Curve(std::string name, int n_point, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3);
    ~Curve();

    // draw_curve();
    glm::vec3 compute_curve_point(float t);
    void update_values(std::vector<glm::vec3>* values, Deformations* deform);

    // float get_L(int element_id);
    MaterialProperty* get_property(int element_id){return &elements[element_id];};;
    void set_id_offset(int new_id_offset){id_offset=new_id_offset;};

  public:
    std::string name;
    int n_points;
    int n_elements; // 1 element or more
    int id_offset;

    std::vector<glm::vec3> curve_points;
  private:

    std::vector<glm::vec3> control_points;
    std::vector<MaterialProperty> elements; // Petit Template !!!
};

#endif // !CURVE_H
