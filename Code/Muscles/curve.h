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
#include "Geometry/element.h"
#include "Tools/tools.h"

class Curve
{
  public:
    Curve(){};
    Curve(std::string name, int n_element, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2, glm::vec3 P3);
    ~Curve();

    // draw_curve();
    glm::vec3 compute_curve_point(float t);

    std::string name;

    std::vector<glm::vec3> curve_points;
  private:
    int n_element; // 1 element or more
    int n_point;
    std::string type; // choice ["bar"] (beam to do)

    std::vector<glm::vec3> control_points;
    // std::vector<Element> element; // Petit Template !!!
};

#endif // !CURVE_H
