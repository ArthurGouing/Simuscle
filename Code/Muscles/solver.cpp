#ifndef SOLVER_CPP
#define SOLVER_CPP
#include "solver.h"

using namespace Eigen;
using namespace glm;

// void Cache::init_cache(Deformations q_points, int nb_frames)
// {
//   _nb_frames = nb_frames;
//   _nb_points = q_points.size();
//   for (int t = 0; t < nb_frames; t++) {
//     for (int i = 0; i < q_points.size(); i++) {
//       q_points[i].pos.z += 0.000001*t * (i*i - 12*i);
//     }
//     cache_values.push_back(q_points);
//   }
// }
// 
// void Cache::fill_cache(VectorXd<float>* q, int frame)
// {
//   // q doit etre de taille 6*i
//   for (int i = 0; i < _nb_points; i++) {
//     cache_values[frame][i].pos.x = q[6*i];
//     cache_values[frame][i].pos.y = q[6*i+1];
//     cache_values[frame][i].pos.z = q[6*i+2];
//     cache_values[frame][i].rot.x = q[6*i+3];
//     cache_values[frame][i].rot.y = q[6*i+4];
//     cache_values[frame][i].rot.z = q[6*i+5];
//     
//   }
// }

Solver::Solver(int n_points, Curve* curve):
  n_points(n_points), n_ddl_tot(6*n_points), n_free_ddl(6*(n_points-2)), n_const_ddl(6*2),
  n_substep(1), kmax(30), _solution(n_points, Qpoint(vec3(0.f), vec3(0.f))),
  _curve(curve),
  impulse(false), gravity(true)
{
  epsilon = pow(10, -2) * _curve->get_property(0)->get_L(); // L de l'ordre du cm (un peu moins pour 10-20 elements -> epsilon de l'ordre du 10eme de mm)
  q.resize(n_free_ddl);
  q_p.resize(12);
  K.resize(n_free_ddl, n_free_ddl);
  K_lp.resize(n_free_ddl, 12);
  F.resize(n_free_ddl, 1);

  init_solver(n_points);
  std::cout<<"update_K: "<<std::endl;
  update_K();

  std::cout<<"Solver built with sucess"<<std::endl;
}

void Solver::init_solver(int n_points)
{
  // En gros on resize tout ici
}

void Solver::update_K()
{
  // Déclatration des variables
  MaterialProperty *property_e1, *property_e2; // The parameters of the beam element. // METTRE l DANS LE PROPERTYY !!!!
  SparseMatrix<float> Ke_1(12, 12); // 
  SparseMatrix<float> Ke_2(12, 12);
  typedef Triplet<float> T;
  std::vector<T> tripletList;
  std::vector<T> tripletList_K_lp;

  // Initialisation
  property_e1 = _curve->get_property(0);
  Ke_1 = build_Ke_glo(property_e1);
  // Resisze K if nedded: or always ?? not a big overhead

  // First point and K_Lp
  for (int ddl_i =  0; ddl_i < 6; ddl_i++) {
    for (int ddl_j = 0; ddl_j < 6; ddl_j++) {
      // Add element to K
      tripletList.push_back(T(ddl_i, ddl_j, Ke_1.coeffRef(ddl_i+6, ddl_j+6)));
      // Add element to K_lp
      tripletList_K_lp.push_back(T(ddl_i, ddl_j, Ke_1.coeffRef(ddl_i+6, ddl_j)));//le block en bas à gauche de Ke (6x6)
    }
  }
  // Build the Stifness matrix K (or rigidity matrix ??)
  for (int id_pts = 0; id_pts < _curve->n_points-3; id_pts++) // Pas sur qu'on ait les bone preperty param
  {
    Info_Print("id: "+std::to_string(id_pts));
    // Build Ke
    if (id_pts+1 >=_curve->n_points) {
      Ke_2.setZero();
    } else {
      property_e2 = _curve->get_property(id_pts+1); 
      Ke_2 = build_Ke_glo(property_e2); 
    }

    // Add Ke to K
    for (int ddl_i = 0; ddl_i < 12; ddl_i++)
    {
      for (int ddl_j = 0; ddl_j < 12; ddl_j++) 
      {
        int i = 6 * id_pts + (ddl_i);
        int j = 6*id_pts + (ddl_j);

        float value = /*Ke_1.coeffRef(6+ddl_i, 6+ddl_j) + */  Ke_2.coeffRef(ddl_i, ddl_j);
        tripletList.push_back(T(i, j, value));
      }
    }
    Ke_1 = Ke_2;
  }
  // Lastpoint and K_lp
  property_e1 = _curve->get_property(_curve->n_elements-1);
  Ke_1 = build_Ke_glo(property_e1);
  for (int ddl_i =  0; ddl_i < 6; ddl_i++) {
    for (int ddl_j = 0; ddl_j < 6; ddl_j++) {
      // Add element to K
      Info_Print("");
      Info_Print(std::to_string(n_free_ddl));
      Info_Print(std::to_string((_curve->n_points-3)*6 + ddl_i) + " " + std::to_string((_curve->n_points-3)*6 + ddl_j));
      tripletList.push_back(T((_curve->n_points-3)*6 + ddl_i, (_curve->n_points-3)*6 + ddl_j, Ke_1.coeffRef(ddl_i, ddl_j)));
      // Add element to K_lp
      tripletList_K_lp.push_back(T((_curve->n_points-3)*6+ddl_i, 6 + ddl_j, Ke_1.coeffRef(ddl_i, ddl_j+6)));//le block en bas à gauche de Ke (6x6)
    }
  }

  K.setFromTriplets(tripletList.begin(), tripletList.end());
  K_lp.setFromTriplets(tripletList_K_lp.begin(), tripletList_K_lp.end());

  std::cout<<" "<<std::endl;
  std::cout<<"K_lp:  "<<std::endl;
  std::cout << MatrixXf(K_lp) << std::endl;

  std::cout<<" "<<std::endl;
  std::cout<<"K:  "<<std::endl;
  std::cout << MatrixXf(K) << std::endl;

  // BUild K_lp
}

SparseMatrix<float> fBcis_y(float L, float xi)
{
  typedef Triplet<float> T;
  std::vector<T> tripletList;
  SparseMatrix<float> Bcis_y(1, 12);

  tripletList.push_back(T(0, 1, -1.f/L));
  tripletList.push_back(T(0, 5, -(1.f-xi)/2.f));
  tripletList.push_back(T(0, 7,  1.f/L));
  tripletList.push_back(T(0,11, -(1.f+xi)/2.f));
  Bcis_y.setFromTriplets(tripletList.begin(), tripletList.end());

  return Bcis_y;
}

SparseMatrix<float> fBcis_z(float L, float xi)
{
  typedef Triplet<float> T;
  std::vector<T> tripletList;
  SparseMatrix<float> Bcis_z(1, 12);

  tripletList.push_back(T(0, 2, -1.f/L));
  tripletList.push_back(T(0, 4, (1.f-xi)/2.f));
  tripletList.push_back(T(0, 8,  1.f/L));
  tripletList.push_back(T(0,10, (1.f+xi)/2.f));

  Bcis_z.setFromTriplets(tripletList.begin(), tripletList.end());

  return Bcis_z;
}

SparseMatrix<float> Solver::build_Ke_glo(MaterialProperty* property)
{
  // Sparse matrix not mendatory to build Ke as Ke is 12x12 matrix and maybe half are zero
  // Declaration des variables
  typedef Triplet<float> T;
  std::vector<T> tripletList;
  SparseMatrix<float> Ke_loc(12, 12), Ke_glo(12, 12), Re(12, 12);
  SparseMatrix<float> Btrac(1, 12), Bcis_y(1, 12) , Bcis_z(1, 12); // the 6 rows of the B matrix
  SparseMatrix<float> Btor(1, 12) , Bflex_y(1, 12), Bflex_z(1, 12);// the 6 rows of the B matrix
  float L = property->get_L(); // L the lenth of the element
  float E = property->get_E();
  float A = property->get_A();
  float G = property->get_G();
  float k = property->get_k();
  float J = property->get_J();
  float Iy = property->get_Iy();
  float Iz = property->get_Iz();
  auto L1 = [](float xi) -> float {return (1.f-xi)/2.f;}; // Lagrange polynome 
  auto L2 = [](float xi) -> float {return (1.f-xi)/2.f;}; // Lagrange polynome
  float int_pt = 1.f/sqrt(3.f);

  // Build Btrac
  tripletList.push_back(T(0, 0, -1.f/L));
  tripletList.push_back(T(0, 6,  1.f/L));
  Btrac.setFromTriplets(tripletList.begin(), tripletList.end());
  tripletList.clear();
  // Build Bcis_y
  Bcis_y = fBcis_y(L, 0.f);

  // Build Bcis_z
  Bcis_z = fBcis_z(L, 0.f);

  // Build Btor
  tripletList.push_back(T(0, 3, -1.f/L));
  tripletList.push_back(T(0, 9,  1.f/L));
  Btor.setFromTriplets(tripletList.begin(), tripletList.end());
  tripletList.clear();
  
  // Build Bflex_y
  tripletList.push_back(T(0, 4, -1.f/L));
  tripletList.push_back(T(0,10,  1.f/L));
  Bflex_y.setFromTriplets(tripletList.begin(), tripletList.end());
  tripletList.clear();
  
  // Build Bflex_z
  tripletList.push_back(T(0, 5, -1.f/L));
  tripletList.push_back(T(0,11,  1.f/L));
  Bflex_z.setFromTriplets(tripletList.begin(), tripletList.end());
  tripletList.clear();

  Ke_loc = E*A*L/2.f   * Btrac.transpose() * Btrac
       + k*G*A*L/2.f * (fBcis_y(L, -int_pt).transpose() * fBcis_y(L, -int_pt) + fBcis_y(L, int_pt).transpose() * fBcis_y(L, int_pt))
       + k*G*A*L/2.f * (fBcis_z(L, -int_pt).transpose() * fBcis_z(L, -int_pt) + fBcis_z(L, int_pt).transpose() * fBcis_z(L, int_pt))
       + J*G*L/2.f   * Btor.transpose() * Btor
       + E*Iy*L/2.f  * Bflex_y.transpose() * Bflex_y
       + E*Iz*L/2.f  * Bflex_z.transpose() * Bflex_z;

  std::cout<<" "<<std::endl;
  std::cout<<"Ke_loc:  "<<std::endl;
  std::cout << MatrixXf(Ke_loc) << std::endl;

  // Compute the stifness matrix in the global coordonate system
  Re = build_Re(property->get_direction());
  Ke_glo = Re.transpose() * Ke_loc * Re;

  std::cout<<" "<<std::endl;
  std::cout<<"Ke_glo:  "<<std::endl;
  std::cout << MatrixXf(Ke_glo) << std::endl;

  return Ke_glo;
}

SparseMatrix<float> Solver::build_Re(vec3 dir)
{
  // Declaration des variables
  SparseMatrix<float> Re(12, 12);

  typedef Triplet<float> T;
  std::vector<T> tripletList;
  vec3 ex = vec3(1.f, 0.f, 0.f);
  vec3 axe;
  float angle;
  Vector3f rot_axis(3);
  Matrix3f re(3,3);

  // Compute the base rotation
  dir   = normalize(dir);
  axe   = normalize(cross(ex, dir)); // Je comprend pas pk je dois normalize ...
  angle = atan(dot(dir, ex));
  rot_axis << axe.x, axe.y, axe.z;
  re = AngleAxisf(angle, rot_axis);

  // Build Re
  for (int q_inc = 0; q_inc < 4; q_inc++) 
  {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        tripletList.push_back(T(3 * q_inc + i, 3 * q_inc + j, re.coeffRef(i,j)));
      }
    }
  }
  Re.setFromTriplets(tripletList.begin(), tripletList.end());
  tripletList.clear();

  std::cout<<" "<<std::endl;
  std::cout<<"Re:  "<<std::endl;
  std::cout << MatrixXf(Re) << std::endl;

  return Re;
}


void Solver::re_build_F(Qpoint q_0, Qpoint q_np1)
{
  // Initialisation
  F.setZero();
  if(gravity)
  {
    F.coeffRef(0) = 0;
  }
  if (impulse)
  {
    if (n_points%2==0){
      F.coeffRef(n_points/2*6 + 2) += -600.f;
    } else {
      F.coeffRef(n_points/2*6 + 2) += -300.f;
      F.coeffRef((n_points/2 + 1)*6 + 2) += -300.f;
    }
    impulse = false;
  }

  q_p.coeffRef(0) = q_0.pos.x; // rotation à faire
  q_p.coeffRef(1) = q_0.pos.y;
  q_p.coeffRef(2) = q_0.pos.z;
  q_p.coeffRef(6) = q_np1.pos.x;
  q_p.coeffRef(7) = q_np1.pos.y;
  q_p.coeffRef(8) = q_np1.pos.z;
  F = F - K_lp * q_p;
}


void Solver::solve_iteration(Qpoint q_0, Qpoint q_np1)
{
  Info_Print("Solve the system");
  // for all substep: 
  // Compute new F
  re_build_F(q_0, q_np1);
  // Eigen solver
  ResMin(); // en vrai il y a que F et x0 qui change d'une itération à l'autre. Mais j'ai envie de laisser K pour la lisibilité...
  _solution.update_deform(q, q_p);
}

void Solver::ResMin (/*const SparseMatrix<float>& A,const VectorXd& b, const VectorXd& x,const double epsilon,const int kmax, VectorXd & x */)
{
  //Déclaration des variables
  int      k;
  float   alpha;
  SparseVector<float> r(n_free_ddl),z(n_free_ddl);

  //Initialisation
  k = 0;
  r = F-K*q;
  // x = x0; // déjà init à x0
  // std::cout<<"r_initiale ="<<r.norm()<<std::endl;
  // std::cout<<"Mean Error_initiale ="<<r.norm()/(float(n_free_ddl))<<std::endl;

  // std::cout<<"Compute..."<<std::endl;

  //Boucle
  while ((r.norm()>epsilon) && (k<=kmax))
  {
    z     = K*r;
    alpha = r.dot(z) / z.dot(z);
    q     = q + alpha*r;
    r     = r - alpha*z;  //résidu
    k    +=1;             //incrémentation itération
  }

  std::cout<<"r ="<<r.norm()<<std::endl;
  std::cout<<"Mean Error ="<<r.norm()/(float(n_free_ddl))<<std::endl;
  std::cout<<"Nombre d'itérations ="<<k<<std::endl;

  // if (k>kmax)
  // {
  //   std::cout<<"Tolérance non atteinte: "<<std::endl;
  // }

  // transforme x to a deformations
}

#endif // !SOLVER_CPP
