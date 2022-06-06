
#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets/QtWidgets>
#include <math.h>
#include "matrix.h"

class triangle
{
public:
  Point p1, p2, p3;
  double min_dist;
  //double max_dist;
  triangle (Point &A, Point &B, Point &C, double min_dst)
  {
    p1.SetCoordinates (A.x, A.y, A.z);
    p2.SetCoordinates (B.x, B.y, B.z);
    p3.SetCoordinates (C.x, C.y, C.z);
    min_dist = min_dst;
  }
  ~triangle () = default;
  triangle (triangle&& p) = default;
  triangle& operator= (triangle&& p) = default;
  triangle (const triangle &p)
  {
    p1.SetCoordinates (p.p1.x, p.p1.y, p.p1.z);
    p2.SetCoordinates (p.p2.x, p.p2.y, p.p2.z);
    p3.SetCoordinates (p.p3.x, p.p3.y, p.p3.z);
    min_dist = p.min_dist;
  }
  triangle& operator= (const triangle& p)
  {
    p1.SetCoordinates (p.p1.x, p.p1.y, p.p1.z);
    p2.SetCoordinates (p.p2.x, p.p2.y, p.p2.z);
    p3.SetCoordinates (p.p3.x, p.p3.y, p.p3.z);
    min_dist = p.min_dist;
    return *this;
  }
};

class Window : public QWidget
{
  Q_OBJECT

private:
  int func_id;
  int nx, ny, p;
  int hole_i, hole_j;
  double shift, hx, hy;
  double *x = nullptr;
  double *u = nullptr;
  double *r = nullptr;
  double *v = nullptr;
  double *b = nullptr;
  double *A = nullptr;
  int *I = nullptr;
  double eps;
  char *filename = nullptr;
  const char *f_name;
  double (*f) (double, double);
  double angle = M_PI / 4;
  int angle_cnt = 3;
  Point axis;
  double cos_12 = cos (M_PI / 12);
  double sin_12 = sin (M_PI / 12);
  double coef = 400;
  double sin_a, cos_a;
  double z_coef = 0.5;
  double step_coef = 0.7;
  double axis_coef = 1;
  Point P1, P2, P3, P4, P5, P6;
  int line_up = 0;
public:
  Window (QWidget *parent, char *in_filename, int in_func_id, int in_nx, int in_ny, double in_eps, int in_p,
                           Point &in_P1, Point &in_P2, Point &in_P3, Point &in_P4, Point &in_P5, Point &in_P6);
  ~Window ();

  QSize minimumSizeHint () const;
  QSize sizeHint () const;
  void redeclare_memory ();
  
  struct comparator
  {
    inline bool operator() (triangle& tr1, triangle& tr2)
      {
        return (tr1.min_dist > tr2.min_dist);
      }
  };
  
public slots:
  void set_func ();
  void increase_angle ();
  void decrease_angle ();
  void increase_z_coef ();
  void decrease_z_coef ();
  void increase_n ();
  void decrease_n ();
  void increase_zoom ();
  void decrease_zoom ();
  void change_line_up ();
protected:
  void paintEvent (QPaintEvent *event);
};

#endif
