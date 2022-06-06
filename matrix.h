#ifndef MATRIX_H
#define MATRIX_H

class Point_2d
{
public:
  double x = 0;
  double y = 0;
  Point_2d () = default;
  Point_2d (double u, double v);
  ~Point_2d () = default;
  Point_2d (Point_2d&& x) = default;
  Point_2d& operator= (Point_2d&& x) = default;
  void SetCoordinates (double u, double v);
};

class Point
{
public:
  double x = 0;
  double y = 0;
  double z = 0;
  Point () = default;
  Point (double u, double v, double t);
  ~Point () = default;
  void SetCoordinates (double u, double v, double t);
};

double get_scalar_product_3d (Point &A, Point &B);
double determinant_3d (Point &A, Point &B, Point &C);
double get_len (Point &P);
double get_dist_3d (double x1, double y1, double z1, double x2, double y2, double z2);
double get_dist_3d (Point &A, Point &B);
double get_dist_2d (double x1, double y1, double x2, double y2);
double dist_to_plane (Point &P, double A, double B, double C, double D);
Point_2d project_without_rotate (Point &P, double angle, double coef, Point &axis, double z_coef);
void rotate_projected (Point_2d &P, double rot_cos, double rot_sin);   

#endif
