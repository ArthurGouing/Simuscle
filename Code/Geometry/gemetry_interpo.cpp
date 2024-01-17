#ifndef GEOMETRY_INTERPO_CPP
#define GEOMETRY_INTERPO_CPP

#include "geometry.h"

#include "Render/renderer.h"
// DebugRenderer::add_line(glm::vec3(0.f,0.f,0.f), glm::vec3(1.f,0.f,1.f), glm::vec3(0.f, 1.f, 0.f));
// DebugRenderer::add_point(vert_pos, glm::vec3(0.2f, 0.2f, 0.6f));

using namespace glm;

GeometryInterpo::GeometryInterpo(Curve* curve, Geometry* mesh, Solver* solver):
  name(mesh->name + "_interpo"), n_verts(-1), n_faces(-1), offset_id(0), _curve(curve), _base_mesh(mesh), 
  lagrange2D(), _deformation(solver->get_solution())
{
  pi = 4*atan(1.);
}

void GeometryInterpo::rebuild_interpolator(int n_z, int n_theta)
{
  // Initialisation
  lagrange2D.reset_points(n_z, n_theta); // construit le std::vector<vec3> interpo_points (ou juste point)

  // Create chebychev points and fine the assiciate disstance
  for ( int i = 0; i < n_z; i++) // On fait tous les zi
  {
    float z_i = 0.5 + 0.5 * cos( 2.*(i)*pi / (2*(n_z-1)));// est ce que z = t * L ??
    // on sait que t n'est pas uniformément réparti...
    for (int j = 0; j < n_theta; j++) // on fait rous les thetai
    {
      // 2) Trouver les coordonnées des points d'interpolation
      float theta_j = pi + pi * cos (2.*(j)*pi / (2.*(n_theta-1)));
      Ray r = _curve->get_ray(z_i, theta_j); // dir doit etre normé //TODO:!!!

      // 3) trouver la valeur du mesh à ces coordonnées par un raycast
      float r_ij = length(_base_mesh->ray_intersection(r) - r.origin);
      r_ij = length(_base_mesh->ray_intersection_approx(r) - r.origin); 
      lagrange2D.add_point(i, j, z_i, theta_j, r_ij);

      // vec3 pos = r.origin;
      vec3 color =  vec3(1.f, 0.f, 0.f) + theta_j/(2*pi) * vec3(-1.f, 0.f, 1.f);
      // Vec3_Print("color: ", color);
      //  DebugRenderer::add_point(r.origin + r_ij*r.direction, vec3(0.1f));
    }
  Ray r = _curve->get_ray(z_i, 0.f);
  // DebugRenderer::add_line(r.origin, r.origin + 0.1f*_curve->tangent(z_i), vec3(0.4f));
  }

  lagrange2D.precompute(); // if it make sense ?
}


void GeometryInterpo::create_geometry(int n, int m)
{
  // Init variables and update members
  n_verts = n*m;
  n_faces = 2 * (n-1) * (m-1); // TODO: A vérifier
  int v_id = 0;

  // Build all vertices of the mesh
  for (int i = 0; i < n; i++) {
    float z = float(i)/float(n-1);
    for (int j = 0; j< m; j++) {
      float theta = 2 * pi * float(j)/float(m-1);
      float r = lagrange2D.at(z, theta);
      Ray Oer = _curve->get_ray(z, theta);
      vec3 vert_pos = Oer.origin + r * Oer.direction; // attention er doit etre normé !
      // vert_pos = vec3(6*z, theta, r);

      Vertex vert(v_id, vert_pos);
      vert.set_t(z); // z c'est t en fait, à réécrire
      vertex_list.push_back(vert);
      v_id++;
    }
  }
  int f_id = 0;
  for (int i = 0; i < n-1; i++) {
    for (int j = 0; j < m-1; j++) {
      // (i-1, j-1), (i-1, j), (i, j-1) et (i, j-1), (i-1, j), (i, j)
      // Add face (i-1, j-1), (i-1, j), (i, j-1)
      Vertex *v1, *v2, *v3;
      int id1, id2, id3;
      if (i==n-2) {
        id1 = i   + j*m;      // (i, j)
        id2 = i   + (j+1)*m;  // (i, j+1);
        id3 = 0 + j*m;      //(i+1, j)
      } else {
        id1 = i   + j*m;      // (i, j)
        id2 = i   + (j+1)*m;  // (i, j+1);
        id3 = i+1 + j*m;      //(i+1, j)
      }
      v1 = &vertex_list[id1];
      v2 = &vertex_list[id2];
      v3 = &vertex_list[id3];
      Triangle tri1(f_id, v1, v2, v3);
      face_list.push_back(tri1);
      // Add neighboor info // faire une fonction
      v1->add_face_neighbor(f_id); v2->add_face_neighbor(f_id); v3->add_face_neighbor(f_id);
      v1->add_vert_neighbor(v2->id); v1->add_vert_neighbor(v3->id);
      v2->add_vert_neighbor(v1->id); v2->add_vert_neighbor(v3->id);
      v3->add_vert_neighbor(v1->id); v3->add_vert_neighbor(v2->id);
      f_id++;

      // Add face (i, j-1), (i-1, j), (i, j)
      if (i==n-2) {
        id1 = 0 + j*m;     // (i+1, j);
        id2 = i   + (j+1)*m; // (i, j+1);
        id3 = 0 + (j+1)*m; // (i+1, j+1);
      } else {
        id1 = i+1 + j*m;     // (i+1, j);
        id2 = i   + (j+1)*m; // (i, j+1);
        id3 = i+1 + (j+1)*m; // (i+1, j+1);
      }
      v1 = &vertex_list[id1];
      v2 = &vertex_list[id2];
      v3 = &vertex_list[id3];
      Triangle tri2(f_id, v1, v2, v3);
      face_list.push_back(tri2);
      // Add neighboor info // faire une fonction
      v1->add_face_neighbor(f_id); v2->add_face_neighbor(f_id); v3->add_face_neighbor(f_id);
      v1->add_vert_neighbor(v2->id); v1->add_vert_neighbor(v3->id);
      v2->add_vert_neighbor(v1->id); v2->add_vert_neighbor(v3->id);
      v3->add_vert_neighbor(v1->id); v3->add_vert_neighbor(v2->id);
      f_id++;
    
      //TODO faire la fermeture selon x
    }
  }
  compute_normals();
  // compute_curve_projection(); // déjà fait à la ligne 57 (-46)
}

void GeometryInterpo::compute_normals()
// Ca va pas du tout...
{
  for (int i = 0; i < n_faces; i++)
    face_list[i].compute_normal();
 
  for (int i = 0; i < n_verts; i++)
    vertex_list[i].compute_vert_normal(&face_list);
}

void GeometryInterpo::compute_curve_projection()
{
  for(auto& v : vertex_list) {
    // Find the clothes point on the curve using the Newton methode
    Info_Print("\nVertex: "+std::to_string(v.id));
    // Declaration des variables
    vec3 Cti, dt_Cti, dtt_Cti;
    float f_ti, ti;
    float eps;
    int k, k_max;
    //
    // Initialisation
    eps = 1.e-3;
    k_max = 50;
    k = 0;
    // Init variables
    ti = 0.5;
    Cti = _curve->compute_curve_point(ti);
    dt_Cti = _curve->tangent(ti);
    dtt_Cti = _curve->derive_second(ti);
    f_ti = dot(dt_Cti, v.pos - Cti);
    while ((abs(f_ti) > eps) and (k<=k_max)) {
      // here, f(ti) = <C'(ti), vi-C(ti)>
      ti += f_ti / (dot(dtt_Cti, v.pos - Cti) + dot(dt_Cti, dt_Cti));
      Cti = _curve->compute_curve_point(ti);
      dt_Cti = _curve->tangent(ti); // Il faudra rajouter la déformé
      dtt_Cti = _curve->derive_second(ti);
      f_ti = dot(dt_Cti, v.pos - Cti);
      k++;
    }
    // Vec3_Print("v = ", v.pos);
    // Vec3_Print("C(ti) = ", Cti);
    Info_Print(std::to_string(k)+": ti="+std::to_string(ti)+"   f_ti="+std::to_string(f_ti));
    v.t = ti;
  }
}

void GeometryInterpo::update_solution(Deformations* new_solution) 
// This function is called just after the computation of new solution in muscle.solve()
{
  _deformation = new_solution;
}

vert_arr GeometryInterpo::compute_value(int id)
{
  Vertex v = vertex_list[id]; // TODO: en pointeur ?
  vert_arr vert;
  // if (id<n_verts/2){
  //   vert.pos=v.pos;
  //   vert.normal=v.normal;
  //   return vert;
  // }

  vert.pos    = _deformation->apply_rotation(v.pos, _curve->compute_curve_point(v.t), v.t) + _deformation->at(v.t).pos; //apply_rotation(vec3 rotated_vector, vec3 rotation_origin)
  // vert.normal = _deformation->apply_rotation(v.normal, _curve->compute_curve_point(v.t), v.t) + _deformation->at(v.t).pos; // Fiare une autre fonction sumpa pour la deformamtion en translation
  vert.normal = v.normal;
  // vert.normal = _deformation->at(v.t).pos + v.pos;
  return vert;
}

GeometryInterpo::~GeometryInterpo()
{}

#endif // !GEOMETRY_INTERPO_CPP
