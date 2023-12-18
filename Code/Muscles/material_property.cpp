#ifndef MATERIAL_PROPERTY_CPP
#define MATERIAL_PROPERTY_CPP

#include "material_property.h"

using namespace glm;

MaterialProperty::MaterialProperty():
  L(),
  rho(1060), // soit une densité de 1,060
  r(0.1),    // 10cm
  E(150000), // 150 kPa
  nu(0.5),   // 0.5 as volume preservation is supposed
  k(6.f/7.f)     // For a circular section
{
  pi = 4*atan(1.);
  update_properties();
}
  

MaterialProperty::MaterialProperty(vec3 init_pts_1, vec3 init_pts_2, float init_rho, float init_r, float init_E, float init_nu, float init_k):
  pts_1(init_pts_1), pts_2(init_pts_2), rho(init_rho), r(init_r), E(init_E), nu(init_nu), k(init_k)
{
  pi = 4*atan(1.); // Ca fait inutilement 1 pi par muscle TODO: change pi access to define it only once
  compute_L();
  update_properties();
}

void MaterialProperty::compute_L() // et direction
{
  L = length(pts_2-pts_1);
}

void MaterialProperty::update_properties()
{
  // Compute A
  A = pi*r*r;
  // Compute G
  G = E/(2*(1+nu));
  // Compute J 
  J = pi * pow(r, 4)/2;
  // Compute Iy
  Iy = pi * pow(r, 4)/4;
  // Compute Iz
  Iz = pi * pow(r, 4)/4;
}


#endif // !MATERIAL_PROPERTY_CPP
