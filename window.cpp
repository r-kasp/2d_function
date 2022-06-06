
#include <QPainter>
#include <stdio.h>

#include "window.h"
#include "matrix.h"
#include "thread.h"
#include <algorithm>
#include <vector>
#include <iostream>

#define ok 0
#define error -1

static
double f_0 (double x, double y)
{
  (void)x;
  (void)y;
  return 1;
}

static
double f_1 (double x, double y)
{
  (void)y;
  return x;
}

static
double f_2 (double x, double y)
{
  (void)x;
  return y;
}

static
double f_3 (double x, double y)
{
  return x + y;
}

static
double f_4 (double x, double y)
{
  return sqrt (x * x + y * y);
}

static
double f_5 (double x, double y)
{
  return x * x + y * y;
}

static
double f_6 (double x, double y)
{
  return exp (x * x - y * y);
}

static
double f_7 (double x, double y)
{
  return 1.0 / (25 * (x * x + y * y) + 1);
}

/*
int bin_search (int n, double *x, double point)
{
  int l = 0, r = n - 1;
  while (r - l > 1)
    {
      int m = (r + l) / 2;
      if (x[m] > point)
        r = m;
      else
        l = m;
    }
  return l;
}*/


Window::Window (QWidget *parent, char *in_filename, int in_func_id, int in_nx, int in_ny, double in_eps, int in_p,
                                 Point &in_P1, Point &in_P2, Point &in_P3, Point &in_P4, Point &in_P5, Point &in_P6)
  : QWidget (parent)
{
  
  func_id = in_func_id;
  func_id--;
  
  nx = in_nx;
  ny = in_ny;
  eps = in_eps;
  filename = in_filename;
  p = in_p;
  
  axis.SetCoordinates (-coef * sqrt (2), coef * sqrt (2), 0);
  
  cos_a = cos (angle);
  sin_a = sin (angle);
  Point N (cos_a, sin_a, z_coef);
  
  P1 = in_P1;
  P2 = in_P2;
  P3 = in_P3;
  P4 = in_P4;
  P5 = in_P5;
  P6 = in_P6;
  //     ____________
  //    /_/_/_/_/_/_/
  //   /_/_/_/_/_/_/
  //  /_/_/_/_/
  // /_/_/_/_/
  //
  hx = fabs (P6.x - P1.x) / nx;
  hy = fabs (P1.y - P2.y) / ny;
  shift = get_dist_3d (P1, P2) / ny;
  shift = sqrt (shift * shift - hy * hy);
  hole_i = round((P3.x - P2.x) / hx);
  hole_j = round((P4.y - P3.y) / hy);
  /*
  hole_i = hole_j = -1;
  for (int i = 0; i <= nx; i++)
    {
      for (int j = 0; j <= ny; j++)
        {
          //опять путаница с j и i
          if (   P2.x + i * hx + j * shift >= P4.x - eps 
              && P2.y + j * hy <= P4.y + eps)
              {
                hole_i = i;
                hole_j = j;
                break;
              }  
        }
      if (hole_i != -1)
        break;  
    }  
  */
  int len = get_len_msr_offdiag (nx, ny, hole_i, hole_j, p, 0) + get_len_msr_diag (nx, ny, hole_i, hole_j) + 1;
  A = new double[len];
  I = new int[len];
  len = get_len_msr_diag (nx, ny, hole_i, hole_j);
  b = new double[len];
  r = new double[len];
  u = new double[len];
  v = new double[len];
  x = new double[len];
  for (int i = 0; i < len; i++)
    x[i] = 0;

  set_func ();
}

Window::~Window ()
{
  delete [] A;
  delete [] I;
  delete [] b;
  delete [] r;
  delete [] u;
  delete [] v;
  delete [] x;
}

QSize Window::minimumSizeHint () const
{
  return QSize (100, 100);
}

QSize Window::sizeHint () const
{
  return QSize (1000, 1000);
}

static void* thread_func (void *args)
{
  thread_info *arg = (thread_info*) args;
  solve (arg);
  return ok;
}

/// change current function for drawing
void Window::set_func ()
{
  func_id = (func_id + 1) % 8;
  switch (func_id)
    {
      case 0:
        f_name = "f (x) = 1";
        f = f_0;
        break;
      case 1:
        f_name = "f (x) = x";
        f = f_1;
        break;
      case 2:
        f_name = "f (x) = y";
        f = f_2;
        break;
      case 3:
        f_name = "f (x) = x + y";
        f = f_3;
        break;
      case 4:
        f_name = "f (x) = sqrt (x * x + y * y)";
        f = f_4;
        break;
      case 5:
        f_name = "f (x) = x * x + y * y";
        f = f_5;
        break;
      case 6:
        f_name = "f (x) = exp (x * x - y * y)";
        f = f_6;
        break;
      case 7:
        f_name = "f (x) = 1 / (25 (x * x + y * y) + 1)";
        f = f_7;
        break;
    }  
    
  int len = get_len_msr_diag (nx, ny, hole_i, hole_j);
  for (int i = 0; i < len; i++)
    x[i] = 0;
  
  pthread_t *threads = new pthread_t[p];
  thread_info *args = new thread_info[p];
  for (int i = 0; i < p; i++)
    {
      args[i].k = i;
      args[i].p = p;
      args[i].A = A;
      args[i].b = b;
      args[i].r = r;
      args[i].x = x;
      args[i].u = u;
      args[i].v = v;
      args[i].I = I;
      args[i].F = f;
      args[i].i_hole = hole_i; 
      args[i].j_hole = hole_j;
      args[i].eps = eps;
      args[i].nx = nx;
      args[i].ny = ny;
      args[i].scr = shift;
      args[i].hx = hx;
      args[i].hy = hy;
      args[i].len = len;
      args[i].x0 = P2.x;
      args[i].y0 = P2.y;
    }
  for (int i = 0; i < p-1; i++)
    {
      int ret = pthread_create (&threads[i], nullptr, thread_func, (void *)&args[i]);
      if (ret != ok)
        {
          printf("ERROR: CAN'T CREATE THREAD\n");
          delete [] threads;
          delete [] args;
          return;
        }
    }
  thread_info *arg = &args[p - 1];
  solve (arg);
  
  delete [] threads;
  delete [] args;
  
  update ();
}

void Window::increase_n ()
{
  ny *= 2;
  nx *= 2;
  
  hx = fabs (P6.x - P1.x) / nx;
  hy = fabs (P1.y - P2.y) / ny;
  shift = get_dist_3d (P1, P2) / ny;
  shift = sqrt (shift * shift - hy * hy);

  hole_i = round((P3.x - P2.x) / hx);
  hole_j = round((P4.y - P3.y) / hy);
  
  delete [] A;
  delete [] I;
  delete [] b;
  delete [] r;
  delete [] u;
  delete [] v;
  delete [] x;
  
  int len = get_len_msr_offdiag (nx, ny, hole_i, hole_j, p, 0) + get_len_msr_diag (nx, ny, hole_i, hole_j) + 1;
  A = new double[len];
  I = new int[len];
  len = get_len_msr_diag (nx, ny, hole_i, hole_j);
  b = new double[len];
  r = new double[len];
  u = new double[len];
  v = new double[len];
  x = new double[len];
  
  len = get_len_msr_diag (nx, ny, hole_i, hole_j);
  for (int i = 0; i < len; i++)
    x[i] = 0;
  
  pthread_t *threads = new pthread_t[p];
  thread_info *args = new thread_info[p];
  for (int i = 0; i < p; i++)
    {
      args[i].k = i;
      args[i].p = p;
      args[i].A = A;
      args[i].b = b;
      args[i].r = r;
      args[i].x = x;
      args[i].u = u;
      args[i].v = v;
      args[i].I = I;
      args[i].F = f;
      args[i].i_hole = hole_i; 
      args[i].j_hole = hole_j;
      args[i].eps = eps;
      args[i].nx = nx;
      args[i].ny = ny;
      args[i].scr = shift;
      args[i].hx = hx;
      args[i].hy = hy;
      args[i].len = len;
      args[i].x0 = P2.x;
      args[i].y0 = P2.y;
    }
  for (int i = 0; i < p-1; i++)
    {
      int ret = pthread_create (&threads[i], nullptr, thread_func, (void *)&args[i]);
      if (ret != ok)
        {
          printf("ERROR: CAN'T CREATE THREAD\n");
          delete [] threads;
          delete [] args;
          return;
        }
    }
  thread_info *arg = &args[p - 1];
  solve (arg);
  
  delete [] threads;
  delete [] args;
  
  update ();
}

void Window::decrease_n ()
{
  if (ny > 1 && nx > 1)
    {
      ny /= 2;
      nx /= 2;
    }
  else return;
  
  hx = fabs (P6.x - P1.x) / nx;
  hy = fabs (P1.y - P2.y) / ny;
  shift = get_dist_3d (P1, P2) / ny;
  shift = sqrt (shift * shift - hy * hy);

  hole_i = round((P3.x - P2.x) / hx);
  hole_j = round((P4.y - P3.y) / hy);


  delete [] A;
  delete [] I;
  delete [] b;
  delete [] r;
  delete [] u;
  delete [] v;
  delete [] x;
  
  int len = get_len_msr_offdiag (nx, ny, hole_i, hole_j, p, 0) + get_len_msr_diag (nx, ny, hole_i, hole_j) + 1;
  A = new double[len];
  I = new int[len];
  len = get_len_msr_diag (nx, ny, hole_i, hole_j);
  b = new double[len];
  r = new double[len];
  u = new double[len];
  v = new double[len];
  x = new double[len];
  
  len = get_len_msr_diag (nx, ny, hole_i, hole_j);
  for (int i = 0; i < len; i++)
    x[i] = 0;
  
  pthread_t *threads = new pthread_t[p];
  thread_info *args = new thread_info[p];
  for (int i = 0; i < p; i++)
    {
      args[i].k = i;
      args[i].p = p;
      args[i].A = A;
      args[i].b = b;
      args[i].r = r;
      args[i].x = x;
      args[i].u = u;
      args[i].v = v;
      args[i].I = I;
      args[i].F = f;
      args[i].i_hole = hole_i; 
      args[i].j_hole = hole_j;
      args[i].eps = eps;
      args[i].nx = nx;
      args[i].ny = ny;
      args[i].scr = shift;
      args[i].hx = hx;
      args[i].hy = hy;
      args[i].len = len;
      args[i].x0 = P2.x;
      args[i].y0 = P2.y;
    }
  for (int i = 0; i < p-1; i++)
    {
      int ret = pthread_create (&threads[i], nullptr, thread_func, (void *)&args[i]);
      if (ret != ok)
        {
          printf("ERROR: CAN'T CREATE THREAD\n");
          delete [] threads;
          delete [] args;
          return;
        }
    }
  thread_info *arg = &args[p - 1];
  solve (arg);
  
  delete [] threads;
  delete [] args;
  
  update ();  
}

void Window::increase_angle ()
{
  angle_cnt = (angle_cnt + 1) % 24;
  angle = angle_cnt * M_PI / 12;

  cos_a = cos (angle);
  sin_a = sin (angle);
  
  double x = axis.x, y = axis.y;
  axis.x = cos_12 * x - sin_12 * y;
  axis.y = sin_12 * x + cos_12 * y;
  
  update ();
}

void Window::decrease_angle ()
{
  angle_cnt--;
  if (angle_cnt == -1)
    angle_cnt += 24;
  std::cout << angle_cnt << std::endl;
  angle = angle_cnt * M_PI / 12;
  
  cos_a = cos (angle);
  sin_a = sin (angle);
  
  double x = axis.x, y = axis.y;
  axis.x = cos_12 * x - sin_12 * y;
  axis.y = sin_12 * x + cos_12 * y;
  
  update ();
}

void Window::increase_z_coef ()
{
  if (z_coef < 1)
    z_coef += 0.1;
  update ();
}

void Window::decrease_z_coef ()
{
  if (z_coef > 0.11)
    z_coef -= 0.1;
  update ();
}

void Window::increase_zoom ()
{
  coef += 20;
  update ();
}

void Window::decrease_zoom ()
{
  if (coef > 20)
    coef -= 20;
  update ();
}

void Window::change_line_up ()
{
  line_up = (line_up + 1) % 3;
  update ();
}


/// render graph
void Window::paintEvent (QPaintEvent * /* event */)
{ 
  QPainter painter (this);
  
  QPen pen_black(Qt::black, 0.5, Qt::SolidLine); 
  QPen pen_red(Qt::red, 2, Qt::SolidLine); 
  QPen pen_green(Qt::green, 2, Qt::SolidLine); 
  QPen pen_blue(Qt::blue, 2, Qt::SolidLine);
  //QPen pen_blue_thick(Qt::blue, 1, Qt::SolidLine); 

  painter.setPen (pen_black);


  // save current Coordinate System
  painter.save ();

  // make Coordinate Transformations
  painter.translate (0.5 * width (), 0.5 * height ());
  
  Point Ox (coef, 0, 0), Oy (0, coef, 0), Oz (0, 0, coef);
  
  Point_2d ox = project_without_rotate (Ox, angle, coef, axis, z_coef);
  Point_2d oy = project_without_rotate (Oy, angle, coef, axis, z_coef);
  Point_2d oz = project_without_rotate (Oz, angle, coef, axis, z_coef);
  
  painter.setPen (pen_blue);
  painter.drawLine (-oz.x, -oz.y, 0, 0);
  //drawing grid
  double delta_x = (P6.x - P1.x) / nx;
  double delta_y = (P1.y - P2.y) / ny;
  painter.setPen (pen_black);
  double scr = get_dist_3d (P1, P2) / ny;
  scr = sqrt (scr * scr - delta_y * delta_y);
  delta_x *= coef; delta_y *= coef; scr *= coef;
  delta_x *= step_coef; delta_y *= step_coef; scr *= step_coef;
  int sz = 128;
  int step = 1;
  if (nx > sz || ny > sz)
    {
      double h = nx * 1.0 / sz;
      step = round (h + 1);
    }
  for (int i = step; i <= nx; i += step)
    {
      for (int j = step; j <= ny; j += step)
        {
          if (i > hole_i && j <= hole_j)
            continue;
          /*
           B D 
          A C
          */
          P2.x *= coef * step_coef; P2.y *= coef * step_coef;
          Point A (P2.x + (i - step) * delta_x + (j - step) * scr, P2.y + (j - step) * delta_y, 0);
          Point B (P2.x + (i - step) * delta_x + j * scr, P2.y + j * delta_y, 0);
          Point C (P2.x + i * delta_x + (j - step) * scr, P2.y + (j - step) * delta_y, 0);
          Point D (P2.x + i * delta_x + j * scr, P2.y + j * delta_y, 0);
          P2.x /= coef * step_coef; P2.y /= coef * step_coef;
          
          Point_2d a = project_without_rotate (A, angle, coef, axis, z_coef);
          Point_2d b = project_without_rotate (B, angle, coef, axis, z_coef);
          Point_2d c = project_without_rotate (C, angle, coef, axis, z_coef);
          Point_2d d = project_without_rotate (D, angle, coef, axis, z_coef);
          
          painter.drawLine (a.x, a.y, b.x, b.y);
          painter.drawLine (a.x, a.y, c.x, c.y);
          painter.drawLine (b.x, b.y, d.x, d.y);
          painter.drawLine (d.x, d.y, c.x, c.y);
          //break;
        }
        //std::cout << std::endl;
        //break;
    }  

  std::vector<triangle> tr;
  
  double residual = 0;
  for (int i = 0; i <= nx; i += step)
    {
      for (int j = 0; j <= ny; j += step)
        {
          if (i > hole_i && j < hole_j)
            continue;
          int l;
          ij2l (nx, ny, hole_i, hole_j, i, j, l);
          double xx = P2.x * coef * step_coef + i * delta_x + j * scr;
          double y = P2.y * coef * step_coef + j * delta_y;
          double f_value = f (xx / step_coef / coef, y / step_coef / coef);
          double value = x[l] - f_value;
          if (fabs (value) > residual)
            residual = fabs (value);
        }
    }    
  printf ("approximation residual : %.6e \n", residual);
  
  if (line_up == 0)
  {
    double max_f = 0;
    for (int i = 0; i <= nx; i++)
      {
        for (int j = 0; j <= ny; j++)
          {
            if (i > hole_i && j < hole_j)
              continue;
            double xx = P2.x * coef * step_coef + i * delta_x + j * scr;
            double y = P2.y * coef * step_coef + j * delta_y;
            double value = f (xx / step_coef / coef, y / step_coef / coef);
            if (fabs (value) > max_f)
              max_f = fabs (value);
          }
      }
    
    int size = 128;
    double delta_x = (P6.x - P1.x) / size;
    double delta_y = (P1.y - P2.y) / size;
    double scr = get_dist_3d (P1, P2) / size;
    scr = sqrt (scr * scr - delta_y * delta_y);
    delta_x *= coef; delta_y *= coef; scr *= coef;
    delta_x *= step_coef; delta_y *= step_coef; scr *= step_coef;

    for (int i = 1; i <= size; i++)
      {
        for (int j = 1; j <= size; j++)
          {
            if (i > hole_i * size * 1.0 / (ny + 1) && j <= hole_j * size * 1.0 / (nx + 1))
              continue;
            double fun_coef = 0.5;
            double x, y, z;
            x = P2.x * coef * step_coef + (i - 1) * delta_x + (j - 1) * scr;
            y = P2.y * coef * step_coef + (j - 1) * delta_y;
            z = f (x / step_coef / coef, y / step_coef / coef);
            Point A (x, y, z / max_f * coef * fun_coef * axis_coef);
            
            x = P2.x * coef * step_coef + (i - 1) * delta_x + j * scr;
            y = P2.y * coef * step_coef + j * delta_y;
            z = f (x / step_coef / coef, y / step_coef / coef);
            Point B (x, y, z / max_f * coef * fun_coef * axis_coef);
            
            x = P2.x * coef * step_coef + i * delta_x + (j - 1) * scr;
            y = P2.y * coef * step_coef + (j - 1) * delta_y;
            z = f (x / step_coef / coef, y / step_coef / coef);
            Point C (x, y, z / max_f * coef * fun_coef * axis_coef);
            
            x = P2.x * coef * step_coef + i * delta_x + j * scr;
            y = P2.y * coef * step_coef + j * delta_y;
            z = f (x / step_coef / coef, y / step_coef / coef);
            Point D (x, y, z / max_f * coef * fun_coef * axis_coef);
            
            double ua = cos_a * A.x + sin_a * A.y + z_coef * A.z - coef;
            double ub = cos_a * B.x + sin_a * B.y + z_coef * B.z - coef;
            double uc = cos_a * C.x + sin_a * C.y + z_coef * C.z - coef;
            double ud = cos_a * D.x + sin_a * D.y + z_coef * D.z - coef;
            
            double da = dist_to_plane (A, cos_a, sin_a, z_coef, -coef);
            double db = dist_to_plane (B, cos_a, sin_a, z_coef, -coef);
            double dc = dist_to_plane (C, cos_a, sin_a, z_coef, -coef);
            double dd = dist_to_plane (D, cos_a, sin_a, z_coef, -coef);
            
            
            if (ua <= 0 && ub <= 0 && uc <= 0)
              {
                double min_dist = std::min (da, std::min (db, dc));
                triangle tmp (A, B, C, min_dist);
                tr.push_back (tmp);
              }
              
            if (ub <= 0 && uc <= 0 && ud <= 0)
              { 
                double min_dist = std::min (db, std::min (dc, dd));
                triangle tmp (B, C, D, min_dist);
                tr.push_back (tmp);
              }
          }
      }    
  }
  else if (line_up == 1)
  {
    double max_f = 0;
    for (int i = 0; i <= nx; i += step)
      {
        for (int j = 0; j <= ny; j += step)
          {
            if (i > hole_i && j < hole_j)
              continue;
            int l;
            ij2l (nx, ny, hole_i, hole_j, i, j, l);
            double value = x[l];
            if (fabs (value) > max_f)
              max_f = fabs (value);
          }
      }
    for (int i = step; i <= nx; i += step)
      {
        for (int j = step; j <= ny; j += step)
          {
            int l = 0;
          
            if (i > hole_i && j <= hole_j + step)
              continue;
            double fun_coef = 0.5;
            double xx, y, z;
            xx = P2.x * coef * step_coef + (i - step) * delta_x + (j - step) * scr;
            y = P2.y * coef * step_coef + (j - step) * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i-step, j-step, l);
            z = x[l];
            Point A (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            xx = P2.x * coef * step_coef + (i - step) * delta_x + j * scr;
            y = P2.y * coef * step_coef + j * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i-step, j, l);
            z = x[l];
            Point B (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            xx = P2.x * coef * step_coef + i * delta_x + (j - step) * scr;
            y = P2.y * coef * step_coef + (j - step) * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i, j-step, l);
            z = x[l];
            Point C (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            xx = P2.x * coef * step_coef + i * delta_x + j * scr;
            y = P2.y * coef * step_coef + j * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i, j, l);
            z = x[l];
            Point D (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            double ua = cos_a * A.x + sin_a * A.y + z_coef * A.z - coef;
            double ub = cos_a * B.x + sin_a * B.y + z_coef * B.z - coef;
            double uc = cos_a * C.x + sin_a * C.y + z_coef * C.z - coef;
            double ud = cos_a * D.x + sin_a * D.y + z_coef * D.z - coef;
            
            double da = dist_to_plane (A, cos_a, sin_a, z_coef, -coef);
            double db = dist_to_plane (B, cos_a, sin_a, z_coef, -coef);
            double dc = dist_to_plane (C, cos_a, sin_a, z_coef, -coef);
            double dd = dist_to_plane (D, cos_a, sin_a, z_coef, -coef);
            
            
            if (ua <= 0 && ub <= 0 && uc <= 0)
              {
                double min_dist = std::min (da, std::min (db, dc));
                triangle tmp (A, B, C, min_dist);
                tr.push_back (tmp);
              }
              
            if (ub <= 0 && uc <= 0 && ud <= 0)
              { 
                double min_dist = std::min (db, std::min (dc, dd));
                triangle tmp (B, C, D, min_dist);
                tr.push_back (tmp);
              }
          }
      }  
  }
  else if (line_up == 2)
  {
    double max_f = residual;
    for (int i = step; i <= nx; i += step)
      {
        for (int j = step; j <= ny; j += step)
          {
            int l = 0;
          
            if (i > hole_i && j <= hole_j + step)
              continue;
            double fun_coef = 0.5;
            double xx, y, z;
            xx = P2.x * coef * step_coef + (i - step) * delta_x + (j - step) * scr;
            y = P2.y * coef * step_coef + (j - step) * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i-step, j-step, l);
            double f_value = f (xx / step_coef / coef, y / step_coef / coef);
            z = fabs (x[l] - f_value);
            Point A (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            xx = P2.x * coef * step_coef + (i - step) * delta_x + j * scr;
            y = P2.y * coef * step_coef + j * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i-step, j, l);
            f_value = f (xx / step_coef / coef, y / step_coef / coef);
            z = fabs (x[l] - f_value);
            Point B (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            xx = P2.x * coef * step_coef + i * delta_x + (j - step) * scr;
            y = P2.y * coef * step_coef + (j - step) * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i, j-step, l);
            f_value = f (xx / step_coef / coef, y / step_coef / coef);
            z = fabs (x[l] - f_value);
            Point C (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            xx = P2.x * coef * step_coef + i * delta_x + j * scr;
            y = P2.y * coef * step_coef + j * delta_y;
            ij2l (nx, ny, hole_i, hole_j, i, j, l);
            f_value = f (xx / step_coef / coef, y / step_coef / coef);
            z = fabs (x[l] - f_value);
            Point D (xx, y, z / max_f * coef * fun_coef * axis_coef);
            
            double ua = cos_a * A.x + sin_a * A.y + z_coef * A.z - coef;
            double ub = cos_a * B.x + sin_a * B.y + z_coef * B.z - coef;
            double uc = cos_a * C.x + sin_a * C.y + z_coef * C.z - coef;
            double ud = cos_a * D.x + sin_a * D.y + z_coef * D.z - coef;
            
            double da = dist_to_plane (A, cos_a, sin_a, z_coef, -coef);
            double db = dist_to_plane (B, cos_a, sin_a, z_coef, -coef);
            double dc = dist_to_plane (C, cos_a, sin_a, z_coef, -coef);
            double dd = dist_to_plane (D, cos_a, sin_a, z_coef, -coef);
            
            
            if (ua <= 0 && ub <= 0 && uc <= 0)
              {
                double min_dist = std::min (da, std::min (db, dc));
                triangle tmp (A, B, C, min_dist);
                tr.push_back (tmp);
              }
              
            if (ub <= 0 && uc <= 0 && ud <= 0)
              { 
                double min_dist = std::min (db, std::min (dc, dd));
                triangle tmp (B, C, D, min_dist);
                tr.push_back (tmp);
              }
          }
      }  
  }
  std::sort (tr.begin (), tr.end(), comparator ());
  //drawing graph
  for (size_t i = 0; i < tr.size (); i++)
    {
      QPainterPath path;
          
      Point A, B, C;
      A.SetCoordinates (tr[i].p1.x, tr[i].p1.y, tr[i].p1.z);
      B.SetCoordinates (tr[i].p2.x, tr[i].p2.y, tr[i].p2.z);
      C.SetCoordinates (tr[i].p3.x, tr[i].p3.y, tr[i].p3.z);
         
      Point_2d a = project_without_rotate (A, angle, coef, axis, z_coef);
      Point_2d b = project_without_rotate (B, angle, coef, axis, z_coef);
      Point_2d c = project_without_rotate (C, angle, coef, axis, z_coef);
        
      //std::cout << a.x << " " << a.y << std::endl;  
      //double coef2 = coef / 2;
      //a.x *= coef2; b.x *= coef2; c.x *= coef2;
      //a.y *= coef2; b.y *= coef2; c.y *= coef2;
      
          
      path.moveTo (a.x, a.y);
      path.lineTo (b.x, b.y);
      path.lineTo (c.x, c.y);
      path.lineTo (a.x, a.y);
          
      painter.setPen (Qt :: NoPen);
      if (line_up == 0)
        painter.fillPath (path, QBrush (QColor ("yellow")));
      else if (line_up == 1)
        painter.fillPath (path, QBrush (QColor ("blue")));
      else
        painter.fillPath (path, QBrush (QColor ("red")));
      
      painter.setPen (pen_black);
      if (line_up == 0)
        pen_black.setColor("orange");  
      else
        pen_black.setColor("black");  
      painter.drawLine (a.x, a.y, b.x, b.y);
      painter.drawLine (b.x, b.y, c.x, c.y);
      painter.drawLine (c.x, c.y, a.x, a.y);
    }    
  
  //drawing axis
  painter.setPen (pen_red);
  painter.drawLine (-ox.x, -ox.y, ox.x, ox.y);
  painter.setPen (pen_green);
  painter.drawLine (-oy.x, -oy.y, oy.x, oy.y);
  painter.setPen (pen_blue);
  painter.drawLine (0, 0, oz.x, oz.y);

  // render function name
  // restore previously saved Coordinate System
  painter.restore ();
  painter.setPen ("black");
  painter.drawText (0, 20, f_name);
  std::string s = "Angle = " + std::to_string (angle_cnt) + " * PI / 12";
  painter.drawText (0, 80, s.c_str ());
  s = "Nx = " + std::to_string (nx);
  painter.drawText (0, 140, s.c_str ());
  s = "Ny = " + std::to_string (ny);
  painter.drawText (0, 200, s.c_str ());
  char buffer[32];
  memset(buffer, 0, sizeof(buffer));
  snprintf(buffer, sizeof(buffer), "%g", residual);
  std::string str(buffer);
  s = "approximation residual = " + str;
  painter.drawText (0, 260, s.c_str ());
}
