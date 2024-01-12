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

Solver::Solver(int n_points, Curve& curve, Solver_param* param):
  impulse(false), gravity(true),
  n_points(n_points), n_ddl_tot(6*n_points), n_free_ddl(6*(n_points-2)), n_const_ddl(6*2),
  _solution(n_points, Qpoint(vec3(0.f), vec3(0.f))), _curve(curve),
  dt(1.f/24.f/float(param->n_substep)), n_substep(param->n_substep), _parameters(param)
{
  resize_solver(n_points);
  L.setZero();

  update_matrices();
}

void Solver::resize_solver(int new_n_points)
{
  n_points = n_points,
  n_ddl_tot = 6*n_points;
  n_free_ddl = 6*(n_points-2);


  // En gros on resize tout ici
  q.resize(n_free_ddl);
  q_n1.resize(n_free_ddl);
  q_p.resize(12);
  A.resize(n_free_ddl, n_free_ddl);
  L.resize(n_free_ddl, n_free_ddl);
  M_1.resize(n_free_ddl, n_free_ddl);
  K_lp.resize(n_free_ddl, 12);
  b.resize(n_free_ddl, 1);

  q.setZero();
  q_n1.setZero();
}

void Solver::update_matrices()
{
  // En implicit en résout x dans le system linéaire: A.x = b
  // En explicit on itère x selon le system: x = A.x + b
  // Update some solver parameters
  n_substep = _parameters->n_substep;
  dt = 1.f/24.f/float(n_substep); // TODO:: à terme, le 24 pourrait changer pour mmatcher les différents formats de fps voulu
  if (_parameters->methode==statics)
  {
    _parameters->n_substep = 1;
    A = build_K();
    if (_parameters->solver==direct)
      cholesky_A();
  }
  else if (_parameters->methode==dynamic_implicit || _parameters->methode==dynamic_explicit)
  {
    // Init
    SparseMatrix<float> Id(n_free_ddl, n_free_ddl), K(n_free_ddl, n_free_ddl); 
    Id.setIdentity();
    // Build matrices
    build_M_1();
    K = build_K();
    // std::cout<<"M_1: "<<std::endl;
    // std::cout<<MatrixXf(M_1)<<std::endl;
    A = Id + dt*dt*M_1*K;
    // std::cout<<"M_1.K: "<<std::endl;
    // std::cout<<MatrixXf(M_1*K)<<std::endl;
    // std::cout<<"A: "<<std::endl;
    // std::cout<<MatrixXf(A)<<std::endl;
    if (_parameters->solver==direct)
      cholesky_A();
    // std::cout<<"LLt: "<<std::endl;
    // std::cout<<MatrixXf(L*L.transpose())<<std::endl;
    // std::cout<<"diff"<<std::endl;
    // std::cout<<MatrixXf(L*L.transpose()-A)<<std::endl;
    exit(0);
  }
  else if (_parameters->methode==dynamic_visc_implicit || _parameters->methode==dynamic_visc_explicit)
  {
    // Init
    SparseMatrix<float> Id(n_free_ddl, n_free_ddl), K(n_free_ddl, n_free_ddl); 
    Id.setIdentity();
    // Build matrices
    build_M_1();
    K = build_K();
    A = Id + (dt*dt/(1.f+dt*dt))* M_1*K;
    if (_parameters->solver==direct)
      cholesky_A();
  }
  else if (_parameters->methode==dynamic_explicit)
  {
    // Init
    SparseMatrix<float> Id(n_free_ddl, n_free_ddl), K(n_free_ddl, n_free_ddl); 
    Id.setIdentity();
    // Build matrices
    build_M_1();
    K = build_K();
    // Compute A
    A = 2.f*Id - dt*dt * M_1*K;
  }
  else if (_parameters->methode==dynamic_visc_explicit)
  {
    // Init
    SparseMatrix<float> Id(n_free_ddl, n_free_ddl), K(n_free_ddl, n_free_ddl); 
    Id.setIdentity();
    // Build matrices
    build_M_1();
    K = build_K();
    // Compute A
    A = 1.f/(1.f+dt*dt) * (2.f*Id - dt*dt* M_1*K);
  } else {
    Err_Print("The method doesn't exist. Choose between ['static', 'dynamic_implicit', 'dynamic_visc_implicit','dynamic_explicit', 'dynamic_visc_implicit']", "solver.cpp");
    exit(1);
  }
}

void Solver::update_b(Qpoint q_0, Qpoint q_np1)
{
  if (_parameters->methode==statics)
  {
    b = build_F(q_0, q_np1);
  }
  else if (_parameters->methode==dynamic_implicit)
  {
    SparseMatrix<float> Id(n_free_ddl, n_free_ddl);
    SparseVector<float> F(n_free_ddl);
    Id.setIdentity();
    F = build_F(q_0, q_np1);
    b = 2.f* q - q_n1 + dt*dt* M_1*F;
  }
  else if (_parameters->methode==dynamic_visc_implicit)
  {
    SparseMatrix<float> Id(n_free_ddl, n_free_ddl);
    SparseVector<float> F(n_free_ddl);
    Id.setIdentity();
    F = build_F(q_0, q_np1);
    b = 2.f/(1+dt*dt)* q - q_n1 + dt*dt/(1+dt*dt)* M_1*F;
  } 
  else if (_parameters->methode==dynamic_explicit)
  {
    SparseVector<float> F(n_free_ddl);
    F = build_F(q_0, q_np1);
    b = dt*dt * M_1*F - q_n1;
    // std::cout << "M_1: "<<std::endl;
    // std::cout << MatrixXf(M_1) << std::endl;
    // std::cout << "q_n1: "<<std::endl;
    // std::cout << VectorXf(q_n1) << std::endl;
    // std::cout << "F: "<<std::endl;
    // std::cout << VectorXf(F) << std::endl;
    // std::cout << "b: "<<std::endl;
    // std::cout << VectorXf(b) << std::endl;
  }
  else if (_parameters->methode==dynamic_visc_explicit)
  {
    SparseVector<float> F(n_free_ddl);
    F = build_F(q_0, q_np1);
    b = dt*dt/(1.f+dt*dt) * M_1*F - q_n1;
  } else {
    Err_Print("The method doesn't exist. Choose between ['static', 'dynamic', 'dynamic_visc']", "solver.cpp");
    exit(1);
  }
}


SparseMatrix<float> Solver::build_K()
{
  // K etant symetrique, on peut la construire 2 fois plus rapidement en construisant qu'une moitié de K, puis en faisant sont sumetrique (ca eviterait d'autre probleme du genre A pas symetrique à cause de 0 qui valent pas 0)
  // Déclatration des variables
  MaterialProperty *property_e1, *property_e2; 
  SparseMatrix<float> K(n_free_ddl, n_free_ddl);
  SparseMatrix<float> Ke_1(12, 12); // 
  SparseMatrix<float> Ke_2(12, 12);
  typedef Triplet<float> T;
  std::vector<T> tripletList;
  std::vector<T> tripletList_K_lp;

  // Initialisation
  property_e1 = _curve.get_property(0);
  Ke_1 = build_Ke_glo(property_e1);
  // Resisze K if nedded: or always ?? not a big overhead

  // First point and K_Lp
  for (int ddl_i =  0; ddl_i < 6; ddl_i++) {
    for (int ddl_j = 0; ddl_j < 6; ddl_j++) {
      // Add element to K
      float value;
      value = Ke_1.coeff(ddl_i+6, ddl_j+6);
      if (abs(value)>0.000001) // 10**-5
        tripletList.push_back(T(ddl_i, ddl_j, value));
      // Add element to K_lp
      value = Ke_1.coeff(ddl_i+6, ddl_j);
      if (abs(value)>0.000001) // 10**-5
        tripletList_K_lp.push_back(T(ddl_i, ddl_j, value));//le block en bas à gauche de Ke (6x6)
    }
  }
  // Build the Stifness matrix K (or rigidity matrix ??)
  for (int id_pts = 0; id_pts < _curve.n_verts-3; id_pts++) // Pas sur qu'on ait les bone preperty param
  {
    // Build Ke
    if (id_pts+1 >=_curve.n_verts) {
      Ke_2.setZero();
    } else {
      property_e2 = _curve.get_property(id_pts+1); 
      Ke_2 = build_Ke_glo(property_e2); 
    }

    // Add Ke to K
    for (int ddl_i = 0; ddl_i < 12; ddl_i++)
    {
      for (int ddl_j = 0; ddl_j < 12; ddl_j++) 
      {
        float value = Ke_2.coeff(ddl_i, ddl_j);
        if (abs(value)<0.000001) // 10**-5
          continue;

        int i = 6 * id_pts + (ddl_i);
        int j = 6*id_pts + (ddl_j);

        tripletList.push_back(T(i, j, value));
      }
    }
    Ke_1 = Ke_2;
  }
  // Lastpoint and K_lp
  property_e1 = _curve.get_property(_curve.n_elements-1);
  Ke_1 = build_Ke_glo(property_e1);
  for (int ddl_i =  0; ddl_i < 6; ddl_i++) {
    for (int ddl_j = 0; ddl_j < 6; ddl_j++) {
      // Add element to K
      tripletList.push_back(T((_curve.n_verts-3)*6 + ddl_i, (_curve.n_verts-3)*6 + ddl_j, Ke_1.coeffRef(ddl_i, ddl_j)));
      // Add element to K_lp
      tripletList_K_lp.push_back(T((_curve.n_verts-3)*6+ddl_i, 6 + ddl_j, Ke_1.coeffRef(ddl_i, ddl_j+6)));//le block en bas à gauche de Ke (6x6)
    }
  }

  K.setFromTriplets(tripletList.begin(), tripletList.end());
  K_lp.setFromTriplets(tripletList_K_lp.begin(), tripletList_K_lp.end());

  // std::cout<<" "<<std::endl;
  // std::cout<<"K_lp:  "<<std::endl;
  // std::cout << MatrixXf(K_lp) << std::endl;

  // std::cout<<" "<<std::endl;
  // std::cout<<"K:  "<<std::endl;
  // std::cout << MatrixXf(K) << std::endl;

  return K;
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

  // std::cout<<" "<<std::endl;
  // std::cout<<"Ke_loc:  "<<std::endl;
  // std::cout << MatrixXf(Ke_loc) << std::endl;

  // Compute the stifness matrix in the global coordonate system
  Re = build_Re(property->get_direction());
  Ke_glo = Re.transpose() * Ke_loc * Re;

  // std::cout<<" "<<std::endl;
  // std::cout<<"Ke_glo:  "<<std::endl;
  // std::cout << MatrixXf(Ke_glo) << std::endl;

  return Ke_glo;
}

void Solver::build_M_1() // TODO faire un update car M_1 est utiliser pour construire F
{
  Info_Print("Build M1");
  //Declaration des variables
  typedef Triplet<float> T;
  std::vector<T> tripletList;

  // Build M_1
  for (int id_pts = 1; id_pts < _curve.n_verts-1; id_pts++) // Pas sur qu'on ait les bone preperty param
  {
    // Info_Print("n_free_ddl: "+std::to_string(n_free_ddl));
    // Info_Print("id_pts: "+std::to_string(id_pts));
    // Info_Print(_curve.name);
    MaterialProperty* p_1 = _curve.get_property(id_pts-1);
    MaterialProperty* p_2 = _curve.get_property(id_pts);

    int i = (id_pts-1)*6;
    float value = p_1->get_rho()*p_1->get_A() + p_2->get_rho()*p_2->get_A();
    tripletList.push_back(T(i, i, 1.f/value));
    tripletList.push_back(T(i+1, i+1, 1.f/value));
    tripletList.push_back(T(i+2, i+2, 1.f/value));
    value = p_1->get_rho()*p_1->get_A()*p_1->get_J() + p_2->get_rho()*p_2->get_A()*p_2->get_J();
    tripletList.push_back(T(i+3, i+3, 1.f/value));
    value = p_1->get_rho()*p_1->get_A()*p_1->get_Iy() + p_2->get_rho()*p_2->get_A()*p_2->get_Iy();
    tripletList.push_back(T(i+4, i+4, 1.f/value));
    value = p_1->get_rho()*p_1->get_A()*p_1->get_Iz() + p_2->get_rho()*p_2->get_A()*p_2->get_Iz();
    tripletList.push_back(T(i+5, i+5, 1.f/value));
  }

  // En théorie Ret*M*Re = M car M est diagonale, d'ou M_1 = (1/Mij)i,j
  M_1.setFromTriplets(tripletList.begin(), tripletList.end());
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

  // std::cout<<" "<<std::endl;
  // std::cout<<"Re:  "<<std::endl;
  // std::cout << MatrixXf(Re) << std::endl;

  return Re;
}


SparseVector<float> Solver::build_F(Qpoint q_0, Qpoint q_np1)
{
  // Initialisation
  SparseVector<float> F(n_free_ddl);
  F.setZero();
  if(_parameters->gravity)
  {
    F.coeffRef(0) = 0;
  }
  if (impulse)
  {
    if (n_points%2==0){
      F.coeffRef(n_points/2*6 + 2) += -6000.f;
    } else {
      F.coeffRef(n_points/2*6 + 2) += -3000.f;
      F.coeffRef((n_points/2 + 1)*6 + 2) += -3000.f;
    }
    impulse = false;
  }

  // Impose p_0 deformation
  Vec3_Print("q_0 rot: ", q_0.rot);
  Vec3_Print("q_np1 rot: ", q_np1.rot);
  q_p.coeffRef(0) = q_0.pos.x; q_p.coeffRef(3) = q_0.rot.x;
  q_p.coeffRef(1) = q_0.pos.y; q_p.coeffRef(4) = q_0.rot.y;
  q_p.coeffRef(2) = q_0.pos.z; q_p.coeffRef(5) = q_0.rot.z;
  // Impose p_n+1 deformation
  q_p.coeffRef(6) = q_np1.pos.x; q_p.coeffRef(9)  = q_np1.rot.x;
  q_p.coeffRef(7) = q_np1.pos.y; q_p.coeffRef(10) = q_np1.rot.y;
  q_p.coeffRef(8) = q_np1.pos.z; q_p.coeffRef(11) = q_np1.rot.z;

  F = F - K_lp * q_p;
  return F;
}


void Solver::solve_iteration(Qpoint q_0, Qpoint q_np1)
{
  Info_Print("Solve the system");
  // for all substep: 
  for (int t = 0; t < _parameters->n_substep; t++) {
    // Compute new F
    update_b(q_0, q_np1);
    // Update values for next timestep
    q_n1 = q; // Update qn-1 before the solver because the solver change directly qn to make it converge to qn+1
    // Solver (il faut choisir le solver avant, pour éviter le if à chaque itération, c'est dégeulasse !)
    if (_parameters->methode==dynamic_explicit || _parameters->methode==dynamic_visc_explicit)
    {
      q = A*q + b;
      continue;
    }
    if (_parameters->solver==iteratif){
      ResMin(); // en vrai il y a que F et x0 qui change d'une itération à l'autre. Mais j'ai envie de laisser K pour la lisibilité...
    } else if (_parameters->solver==direct){
      solveLU();
    } else {
      Err_Print("This solver doesn't exist. Choose beetween ['iteratif' and 'direct']", "solver.cpp");
      exit(1);
    }
  }
  _solution.update_deform(q, q_p);
  // _solution.print();
}

void Solver::cholesky_A()
{
  for (int  j = 0;j < n_free_ddl;j++) { // Pour toutes les colonnes de A
    //Calcul Lii
    float sum_jj=0;
    for (int k = 0; k < j; k++) { // pour toutes les colonnes précédantes
      sum_jj += L.coeffRef(j,k) * L.coeffRef(j, k);
    }
    L.coeffRef(j, j) = sqrt(A.coeffRef(j, j) - sum_jj);
    if (A.coeffRef(j,j)<=sum_jj){ // Pas très opti tout ca...
      Err_Print("Tried to take the square root of a nefative number whil compute L"+std::to_string(j)+","+std::to_string(j)+".\nThe Matrice too solve is not sdp. Use the iterative solver.", "solver.cpp");
      std::cout << "L: "<<std::endl;
      std::cout << MatrixXf(L) << std::endl;
      exit(1);
    }
    //Calculs Lij 
    int sparse_max_borne = (j<n_free_ddl-6) ? 6 + (j/6+1)*6 : n_free_ddl;
    for (int i = j+1; i < sparse_max_borne; i++) { // Pour toutes les lignes de la colonnes j 
                                    // L etant inférieur, On commence à partir de la diagonale
      float sum_ij = 0;

      for (int k = i/12*6; k < j; k++) { // pour toute les colonnes précédantes de la lignes i
        sum_ij += L.coeffRef(i, k) * L.coeffRef(j, k);
      }
      L.coeffRef(i, j) = (A.coeffRef(i, j) - sum_ij) / L.coeffRef(j, j);
    }
  }
  // std::cout<<"L: "<<std::endl;
  // std::cout<<MatrixXf(L)<<std::endl;
}

void Solver::solveLU()
{
  VectorXf z(n_free_ddl);

  // Solver L.z=b
  for (int i = 0; i < n_free_ddl; i++) {
    float sum=0;
    for (int k = 0; k < i; k++) {
      sum += L.coeff(i, k) * z(k);
    }
    z.coeffRef(i) = (b.coeff(i) - sum) / L.coeff(i, i);
  }

  // Solve Lt.q=z
  for (int _i = 0; _i < n_free_ddl; _i++) {
    int i = n_free_ddl-_i-1; // (i va de n à 0)
    float sum=0;
    for (int _k = 0; _k < n_free_ddl-i-1; _k++) {
      int k = n_free_ddl-_k-1;
      sum += L.coeffRef(k, i) * q.coeff(k); // Lt (i,k) = L(k,i)
    }
    q.coeffRef(i) = (z.coeff(i) - sum) / L.coeffRef(i, i);
  }
}

void Solver::ResMin (/*const SparseMatrix<float>& A,const VectorXd& b, const VectorXd& x,const double epsilon,const int kmax, VectorXd & x */)
{
  //Déclaration des variables
  int      k;
  float   alpha;
  SparseVector<float> r(n_free_ddl),z(n_free_ddl);

  //Initialisation
  k = 0;
  r = b-A*q;
  // x = x0; // déjà init à x0
  // std::cout<<"r_initiale ="<<r.norm()<<std::endl;
  // std::cout<<"Mean Error_initiale ="<<r.norm()/(float(n_free_ddl))<<std::endl;

  // std::cout<<"Compute..."<<std::endl;

  //Boucle
  while ((r.norm()>_parameters->epsilon) && (k<=_parameters->kmax))
  {
    z     = A*r;
    alpha = r.dot(z) / z.dot(z);
    q     = q + alpha*r;
    r     = r - alpha*z;  //résidu
    k    +=1;             //incrémentation itération
  }

  // std::cout<<"r ="<<r.norm()<<std::endl;
  // std::cout<<"Mean Error ="<<r.norm()/(float(n_free_ddl))<<std::endl;
  // std::cout<<"Nombre d'itérations ="<<k<<std::endl;

  // if (k>kmax)
  // {
  //   std::cout<<"Tolérance non atteinte: "<<std::endl;
  // }

  // transforme x to a deformations
}

#endif // !SOLVER_CPP
