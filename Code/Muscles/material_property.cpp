#ifndef MATERIAL_PROPERTY_CPP
#define MATERIAL_PROPERTY_CPP

#include "material_property.h"


#include "Render/renderer.h"
// DebugRenderer::add_point(p, glm::vec3(0.2f, 0.2f, 0.6f));
// DebugRenderer::add_line(glm::vec3(0.f,0.f,0.f), glm::vec3(1.f,0.f,1.f), glm::vec3(0.f, 1.f, 0.f));

using namespace glm;

MaterialProperty::MaterialProperty():
  L(),
  rho(1060.f), // soit une densité de 1,060
  r(0.1),    // 10cm
  E(150000), // 150 kPa
  nu(0.5),   // 0.5 as volume preservation is supposed
  k(6.f/7.f)     // For a circular section
{
  pi = 4*atan(1.);
  update_properties();
}
  

MaterialProperty::MaterialProperty(vec3 init_pts_1, vec3 init_pts_2, float init_rho, float init_r, float init_E, float init_nu, float init_k, vec3 seem_dir):
  pts_1(init_pts_1), pts_2(init_pts_2), rho(init_rho), r(init_r), E(init_E), nu(init_nu), k(init_k)
{
  pi = 4*atan(1.); // Ca fait inutilement 1 pi par muscle TODO: change pi access to define it only once
  compute_L();
  update_properties();
  e_z = normalize(init_pts_2-init_pts_1);
  e_theta0 = normalize(cross(e_z, seem_dir));
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

float MaterialProperty::sdf_approx(vec3 p, Interpolator &r)
/* Compute the cloeset point Pc on the segment between and the point
 * then compute the sdf in the plane Pc,er,etheta.
 * This way of computing sdf gove clearly wrong result ,
 * specially if r(z,theta) have big variations. 
 * But I think it should give a good approximation as it is
 * the method used to compute sdf of cylinders
 *
 * It could be a starting point before doing few steps of gradient descent
 * to obtain:
 * 1) closet point 
 * 2) which respect the orthogonalité (the closest point of the surface, is the one, where surface is orthogonal to d)
*/
{
  // Find the closest point on the segment
  vec3 cp;   // The closest_point
  vec3 proj; // projection of p on the segment
  float z, theta, cos_theta; // Coord

  proj = pts_1 + dot(p-pts_1, e_z) * e_z; 

  if (dot(e_z, p-pts_1) <=0) {        // When the projected point is before p1
    cp =  pts_1;
    // z = el_id-1/tot_id;
  } else if (length(pts_1 - proj) >= L) { // When the projected points is after p2
    cp = pts_2;
    // z = el_id/tot_id;
  } else {                       // When the projected point is on the œ
    cp = proj;
    // el_id + dot(p-pts_1, e_z) / tot_id;
  }

  DebugRenderer::add_line(pts_1, pts_2, glm::vec3(0.f, 0.f, 0.f));
  DebugRenderer::add_line(pts_1, pts_1+L/2.f*e_z, glm::vec3(0.f, 0.f, 1.f));
  DebugRenderer::add_line(p, cp, glm::vec3(1.f, 0.f, 0.f));

  // COmpute the distance

  z = dot(p,e_z)*L;
  cos_theta = dot(normalize(p-proj), e_theta0); // evec theta0 normé
  theta = (dot( cross(-e_theta0, (p-proj)) , e_z ) > 0 ) ? 
    acos(cos_theta) + pi/2 : -acos(cos_theta) + pi/2.;

  z = 0.5f; // TODO: proper way to acces t curve values here

  float d = length(p-cp) - r.at(z, theta);
  return d;
}

float MaterialProperty::sdf_approx(MaterialProperty& el2, Interpolator &r)
// TODO: not finished, have to write some of the collider manager ...
{
  // Find the closest point on the segment
  vec3 cp;   // The closest_point
  vec3 proj_el1, proj_el2; // projection of p on the segment
  vec3 n;
  // el2 basic points
  vec3 el2_e_z = el2.get_direction();
  float z1, theta1, cos_theta1; // Coord
  float z2, theta2, cos_theta2; // Coord

  n = cross(e_z, el2_e_z); // WARN: divide by 0 if parallele
  // proj_el1 =  ; // C'est un peu chiant à trouver
  // proj_el2 =  ;
  // Get z, theta de proj_el1 sur el2
  // Get z, theta de proj_el2 sur el1
  //
  // d = length(proj_el2-proj_el1) - (r2(z1, theta1) + r2(z2, theta2))

  return dot(n, pts_1 - el2.pts_1); // Le projeté de pts1_el1-pts1_el2 sur n le vecteur orthogonal au 2 coubes
}

float dist(vec3 p, vec3 B, vec3 d)
{
  return length(cross(p-B, d))/length(d);
}



#endif // !MATERIAL_PROPERTY_CPP
