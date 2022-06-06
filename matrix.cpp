#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "matrix.h"

Point::Point (double u, double v, double t)
{
  x = u; y = v; z = t;
}
  
void Point::SetCoordinates (double u, double v, double t)
{
  x = u; y = v; z = t;
}

Point_2d::Point_2d (double u, double v)
{
  x = u; y = v;
}
  
void Point_2d::SetCoordinates (double u, double v)
{
  x = u; y = v;
}

double determinant_3d (Point &A, Point &B, Point &C)
{
  /*
  A.x A.y A.z
  B.x B.y B.z
  C.x C.y C.z
  */
  return A.x * B.y * C.z + A.y * B.z * C.x + A.z * B.x * C.y -
         A.z * B.y * C.x - A.y * B.x * C.z - A.x * B.z * C.y;
}



double get_len (Point &P)
{
  return sqrt (P.x * P.x + P.y * P.y + P.z * P.z);
}

double get_dist_3d (double x1, double y1, double z1, double x2, double y2, double z2)
{
  return sqrt ( (x1 - x2) * (x1 - x2) 
              + (y1 - y2) * (y1 - y2)
              + (z1 - z2) * (z1 - z2) );
}

double get_dist_3d (Point &A, Point &B)
{
  return sqrt ( (A.x - B.x) * (A.x - B.x) 
              + (A.y - B.y) * (A.y - B.y)
              + (A.z - B.z) * (A.z - B.z) );
}

double get_dist_2d (double x1, double y1, double x2, double y2)
{
  return sqrt ( (x1 - x2) * (x1 - x2) 
              + (y1 - y2) * (y1 - y2) );
}

double get_scalar_product_3d (Point &A, Point &B)
{
  return A.x * B.x + A.y * B.y + A.z * B.z;
}

double dist_to_plane (Point &P, double A, double B, double C, double D)
{
  double len = sqrt (A * A + B * B + C * C);
  if (fabs (len) < 1e-14)
    return 0;
  return fabs (A * P.x + B * P.y + C * P.z + D) / len;
}

//plane is cos_a * x + sin_a * y + z = coef
//coef = sqrt2
Point_2d project_without_rotate (Point &P, double angle, double coef, Point &axis, double z_coef)
{
  Point_2d res (0, 0);
    
  double eps = 1e-7;
  double cos_a = cos (angle), sin_a = sin (angle);
  Point M (P.x, P.y, P.z);
  double N_len = sqrt (1 + z_coef * z_coef);
  if (fabs (N_len) < eps)
    return res;
  Point N (cos_a / N_len, sin_a / N_len, z_coef / N_len);
  double dist = fabs (cos_a * M.x + sin_a * M.y + z_coef * M.z - coef) / N_len;
  
  M.x += N.x * dist;
  M.y += N.y * dist;
  M.z += N.z * dist;
  
  Point center (N.x * coef / N_len, N.y * coef / N_len, N.z * coef / N_len);
  dist = get_dist_3d (center.x, center.y, center.z, M.x, M.y, M.z);
  
  if (fabs (dist) < eps)
    return res;
  //проекция неправильная
  /*
  double v_x = (M.x - center.x) / dist, v_y = (M.y - center.y) / dist;
  dist = get_dist_3d (center.x, center.y, center.z, M.x, M.y, M.z);
  v_x *= dist; v_y *= dist;
  */
  /*
  1. взять в 2д единичный вектор Oz он будет (0, -1) а в реальности считается в трехмерных
  2. посчитать косинус между трехмерным Oz и вектором от центра до проекции
  3. повернуть (0, -1) и умножить на длину вектора в 2)
  */
  Point OZ (0 - center.x, 0 - center.y, coef / z_coef - center.z);
  
  Point V (M.x - center.x, M.y - center.y, M.z - center.z);
  double scalar_product = get_scalar_product_3d (OZ, V);
  double scalar_product2 = get_scalar_product_3d (axis, V);
  double len_OZ = get_len (OZ);
  double len_V = get_len (V);
  if (fabs (len_V) < eps || fabs (len_OZ) < eps)
    return res;
  double cos_b = (scalar_product / len_OZ) / len_V;
  if (cos_b > 1)
    cos_b = 1;
  if (cos_b < -1)
    cos_b = -1;
  double sin_b = sqrt (1 - cos_b * cos_b);
  if (fabs (scalar_product2) < eps)
    {
      if (scalar_product > 0)
        res.y = -len_V;
      else
        res.y = len_V;
      return res;
    }
  if (scalar_product2 < 0)
    sin_b *= -1;
  res.y = -1; // res = {0, -1}
  rotate_projected (res, cos_b, sin_b);
  res.x *= len_V;
  res.y *= len_V;
  return res;
}


void rotate_projected (Point_2d &P, double rot_cos, double rot_sin)    
{
  double old_x = P.x, old_y = P.y;
  P.x = old_x * rot_cos - old_y * rot_sin;
  P.y = old_x * rot_sin + old_y * rot_cos;
}



