#ifndef INTERPOLATOR_CPP
#define INTERPOLATOR_CPP

#include "interpolator.h"

using namespace glm;

Interpolator::Interpolator() :
  n_i(-1), n_j(-1)
{
}

void Interpolator::reset_points(int new_n_i, int new_n_j)
{
  n_i = new_n_i;
  n_j = new_n_j;
  points.clear();
}

void Interpolator::add_point(int i, int j, float xi, float xj, float value)
{
  Pij P(i, j, xi, xj, value);
  points.push_back(P);
}

void Interpolator::precompute()
{
  // check if there is the good number of point
  //  and do precomputation if needed ??
}

float Interpolator::lagrange_poly_X(int i, float x, int j)
{
  float l = 1.;
  float xi = points.get_x(i,j); // get all point of the j row
  for (int k = 0; k < n_i; k++) {
    if (k==i) continue;
    float xk = points.get_x(k,j); // get all point of the j row
    l *= (x-xk) / (xi-xk);
  }
  return l;
}
float Interpolator::lagrange_poly_Y(int j, float y, int i)
{
  float l = 1.;
  float yj = points.get_y(i, j);
  for (int k = 0; k < n_j; k++) {  // n_i et n_j ne sont pas très clair, c'est le nb de points selon x et y respectivement
    if (k==j) continue;
    float yk = points.get_y(i, k);
    l *= (y-yk) / (yj-yk);
  }
  return l;
}

float Interpolator::at(float x, float y)
{
  float z = 0;
  for (int i = 0; i < n_i; i++) {
    float li = lagrange_poly_X(i, x, 0);
    for (int j = 0; j < n_j; j++) {
      float lj = lagrange_poly_Y(j, y, i);
      z+= li * lj * points.get_value(i,j); 
      // Comme on a les meme points points de tchebyvhec, les li et lj sont identique et on a bcp de répétition inutile... 
      // on peut par example calculer li en dehors de la boucle en j
    }
  }
  return z;
}

Interpolator::~Interpolator()
{}

#endif // !INTERPOLATOR_CPP
