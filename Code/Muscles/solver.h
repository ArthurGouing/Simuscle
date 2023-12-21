//**********************************************************************************//
// Simuscle by Arthur Gouinguenet
// File: solver.cpp
// Parent: muscle.cpp
// Date: 15/12/23
// Content: class to solve the structural problem
//**********************************************************************************//
#ifndef SOLVER_H
#define SOLVER_H

//******** LIBRARY ******** 
#include "material_property.h"
#include "curve.h"
#include "Tools/tools.h"

#include <cmath>
#include <glm/glm.hpp>
#include "Eigen_src/Sparse"
#include "Eigen_src/Dense"
#include <string>

/**** Deformations is defined in curve.h ****/

// class Cache 
// {
//   public: 
//     Cache(){};
//     ~Cache(){};
// 
//     void init_cache(Deformations q_points, int nb_frames);
// 
//     void fill_cache(Eigen::VectorXd<float>* q, int frame);
//     void clear_cache(); //TODO
// 
//     Deformations* get_deformations(int frame){return &cache_values[frame];};
//     Qpoint        get_Q(int frame, int point_id){return cache_values[frame][point_id];};
// 
//   private:
//     int _nb_frames;
//     int _nb_points;
//     std::vector<Deformations> cache_values; // contient les solutions à chaque frame
// };
//
enum Simu_type   {statics=0, dynamic_implicit=1, dynamic_visc_implicit=2, dynamic_explicit=3, dynamic_visc_explicit=4};
enum Solver_type {iteratif=0, direct=1};

struct Solver_param {
  Solver_param(): 
    gravity(true), n_substep(50), kmax(30), epsilon(0.000000001), methode(0), solver(1) {};

  bool gravity;  // Add gravity force
  bool impulse;  // Add dirac force (add force during 1 timestep so not really a dirac)
  int n_substep; // The number of step within the frame to compute
  int kmax;      // The max number of iteraation of ResMin
  float epsilon; // The tolerance of the solution of ResMin
  // bool impulse; //  ???
  int methode; // Choose beetween ["static", "dynamic", "dynamic_visc"]
  int solver;  // Choose beetween ["iteratif", "direct"]
};


class Solver
{
  public:
    Solver(){};
    Solver(int n_points, Curve& curve, Solver_param* param);
    ~Solver(){};

    void resize_solver(int n_points);

    void update_matrices();
    void build_M_1();
    Eigen::SparseMatrix<float> build_K();
    Eigen::SparseMatrix<float> build_Ke_glo(MaterialProperty* property);
    Eigen::SparseMatrix<float> build_Re(glm::vec3 dir);

    void update_b(Qpoint q_0, Qpoint q_np1);
    Eigen::SparseVector<float> build_F(Qpoint q_0, Qpoint q_np1);

    void solve_iteration(Qpoint q_0, Qpoint q_np1);

    void ResMin();
    void cholesky_A();
    void solveLU();

    // Get/Set functions
    Deformations* get_solution(){return &_solution;};
    // Deformations* get_solution(int frame); use cache
    
    // UI variables
    bool impulse; // to apply an impulse on the muscle
    bool gravity; // to apply gravity to muscele


  private:
    int n_points;
    int n_ddl_tot; // size of $q_f$
    int n_free_ddl; // depend of the element used and the method
    int n_const_ddl; // always 2

    // 
    Eigen::SparseVector<float> q, q_n1; // l'inconnue
    Eigen::SparseMatrix<float> A, L; // L la décomposition de Cholesky de A
    // SparseMatrix<float> M; // useless for static sim
    Eigen::SparseVector<float> b;

    // intermediaire needed matrix for F building
    Eigen::SparseVector<float> q_p; // change when the frame change
    Eigen::SparseMatrix<float> K_lp; // chagne when K change (i.e R||D||B)
    Eigen::SparseMatrix<float> M_1; // ne change jamais ! 
 
    // Solution exploitable
    Deformations _solution; 

    // Curve
    Curve _curve;

    // simulation parameters
    float dt;
    int n_substep;
    Solver_param* _parameters;

    // Solution cache
    // Cache _cache; // eventuellement en poiteur si le cache est dans musclesystem
};

#endif // !SOLVER_H
