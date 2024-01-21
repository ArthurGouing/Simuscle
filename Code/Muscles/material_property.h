//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: material_property.cpp
// Parent: muscle_system.cpp
// Date: 15/12/23
// Content: class to old and compute all the material proerties of the muscle
//**********************************************************************************//
#ifndef MATERIAL_PROPERTY_H
#define MATERIAL_PROPERTY_H

//******** LIBRARY ******** 
#include <cmath>
#include <glm/glm.hpp>
#include "Geometry/interpolator.h"

/* 
 * For now, it contain the property of a circular beam, isotrope.
*/ 
class Curve;

class MaterialProperty  // Rename en beam element // and/or make a struct for olding mechanicals property
{
  public:
    MaterialProperty();
    MaterialProperty(glm::vec3 pts_1, glm::vec3 pts_2, float rho, float r, float E, float nu, float k, glm::vec3 seem_dir);
    ~MaterialProperty(){};

    void update_properties();
    void compute_L();

    // Get functions
    float get_L  (){return L;};
    float get_rho(){return rho;};
    float get_E  (){return E;};
    float get_A  (){return A;};
    float get_k  (){return k;};
    float get_G  (){return G;};
    float get_J  (){return J;};
    float get_Iy (){return Iy;};
    float get_Iz (){return Iz;};

    glm::vec3 get_direction(){return pts_2-pts_1;};
    
    // Rel variables:
    float sdf_approx(glm::vec3 p, Interpolator &r);
    float sdf_approx(MaterialProperty& el, Interpolator &r);

    // Set functions
    void set_rho(float new_rho){rho = new_rho;};
    void set_r(float new_r)    {r = new_r;   update_properties();} // A, J, Iy and Iz depend of r
    void set_E(float new_E)    {E = new_E;   update_properties();} // G depend of E and nu
    void set_nu(float new_nu)  {nu = new_nu; update_properties();} // G depend of E and nu
    void set_k(float new_k)    {k = new_k;}

    void set_pts(glm::vec3 new_pts_1, glm::vec3 new_pts_2, glm::vec3 seem_dir){
      pts_1=new_pts_1; 
      pts_2=new_pts_2;
      compute_L();
      e_z = normalize(new_pts_2-new_pts_1);
      e_theta0 = normalize(cross(e_z, seem_dir));
    };
    // Note After changing MaterialProperty, we must rebuild the matrix K

  private:
    float pi;  // The pi value (3.14)
 
    glm::vec3 pts_1;
    glm::vec3 pts_2;
    glm::vec3 e_z, e_theta0;
    float L;   // The lenth of the element

    float rho; // The volumic mass of the muscle
    float r;   // the mean radius of the muscle section
    float A;   // the Aera of the muscle section
    float E;   // the Young modulus of the muscle
    float nu;  // the Poisson coefficient of the muscle (always 0.5 for volume preservation)
    float G;   // the shear modulus 
    float k;   // the corrective coefficient
    float J;   // The quadratique moment of the section along x axis
    float Iy;  // The quadratique moment along y
    float Iz;  // The quadratique moment along z
};

#endif // !MATERIAL_PROPERTY_H
