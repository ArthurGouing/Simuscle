//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: interpolatro.cpp
// Parent: geometryinterpo.cpp
// Date: 13/01/24
// Content: Class that compute interpolation of function
// Or surface, or analytique mesh surface. could ve template also to use different alg/methode to modelize the surface of the mesh
//**********************************************************************************//
#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

//******** LIBRARY ******** 
#include <cmath>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Tools/tools.h"


struct Pij {
  Pij(int i_init, int j_init, float init_x_i, float init_x_j, float value_init):
    i(i_init), j(j_init), xi(init_x_i), xj(init_x_j), value(value_init) {}
  int i;
  int j;
  float xi;
  float xj;
  float value;
};

struct LagrangePoints {
  std::vector<Pij> point_list;
  void clear() {point_list.clear();};
  void push_back(Pij p) {point_list.push_back(p);};

  float get_x(int i, int j) {
    for(auto& p : point_list) {
      if (i==p.i && j==p.j)
        return p.xi;
    }
    Err_Print("The coord ("+std::to_string(i)+","+std::to_string(j)+") doesn't exist in the list of interpolation points, can't find the assiciated value", "interpolator.h");
    return 1.f;
  };
  float get_y(int i, int j) {
    for(auto& p : point_list) {
      if (i==p.i && j==p.j)
        return p.xj;
    }
    Err_Print("The coord ("+std::to_string(i)+","+std::to_string(j)+") doesn't exist in the list of interpolation points, can't find the assiciated value", "interpolator.h");
    return 1.f;
  };
  float get_value(int i, int j) {
    for(auto& p : point_list) {
      if (i==p.i && j==p.j)
        return p.value;
    }
    Err_Print("The coord ("+std::to_string(i)+","+std::to_string(j)+") doesn't exist in the list of interpolation points, can't find the assiciated value", "interpolator.h");
    return 1.f;
  };
};

//  2D Lagrange interpolator with "1D chebychev point"
class Interpolator // Find a better name
{
  public:
    // Constructor
    Interpolator();

    // Init
    void reset_points(int new_n_i, int new_n_j);
    void add_point(int i,int j, float xi, float xj, float point_value);

    void precompute(); // Needed for optimize Lagrange computation

    // Compute
    float lagrange_poly_X(int i, // lagrange_polynome at point i
                          float x, // at the point x
                          int j   // along the X point of the jth row
                        );
    float lagrange_poly_Y(int j, // lagrange_polynome at point j
                          float y, // at the point y
                          int i   // along the X point of the ith line
                        );
    float at(float x, float y);

    // Destructor
    ~Interpolator();

  public: // Variables
    // Number of interpolation point
    int n_i, n_j; // along x|n_i and y|n_j
    // List of interpolation point
    LagrangePoints points; // A revoir pour pouvoir accéder facilement aux point x_j pour 1<j<n_j et y_i 1<i<n_i

};
#endif // !INTERPOLATOR_H
