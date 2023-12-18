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


class Solver
{
  public:
    Solver(){};
    Solver(int n_points, Curve* curve);
    ~Solver(){};

    void init_solver(int n_points);

    void update_K();
    Eigen::SparseMatrix<float> build_Ke_glo(MaterialProperty* property);
    Eigen::SparseMatrix<float> build_Re(glm::vec3 dir);

    void solve_iteration(Qpoint q_0, Qpoint q_np1);
    void re_build_F(Qpoint q_0, Qpoint q_np1);

    void ResMin(/*Eigen::SparseMatrix<double>& A, Eigen::SparseVector<float>& b, Eigen::SparseVector<float>& x0,
        double epsilon, int kmax, VectorXd & x*/);

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
    Eigen::SparseVector<float> q; // l'inconnue
    Eigen::SparseMatrix<float> K;
    // SparseMatrix<float> M; // useless for static sim
    Eigen::SparseVector<float> F;

    // intermediaire needed matrix for F building
    Eigen::SparseVector<float> q_p; // change when the frame change
    Eigen::SparseMatrix<float> K_lp; // chagne when K change (i.e R||D||B)
 
    // Solution exploitable
    Deformations _solution; 

    // Curve
    Curve* _curve;

    // simulation parameters
    int n_substep;
    int kmax;      // The max number of iteraation of ResMin
    float epsilon; // The tolerance of the solution of ResMin

    // Solution cache
    // Cache _cache; // eventuellement en poiteur si le cache est dans musclesystem
};

#endif // !SOLVER_H
