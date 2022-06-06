#include "thread.h"
#include <cmath>
#include <cstring>
#include <iostream>


void reduce_sum (int p)
{
  static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
  static int t_in = 0;
  static int t_out = 0;
  if (p <= 1)
    return;
  pthread_mutex_lock (&m);
  t_in++;
  if (t_in >= p)
    {
      t_out = 0;
      pthread_cond_broadcast (&c_in);
    }
  else
    {
      while (t_in < p)
        pthread_cond_wait (&c_in, &m);
    }
  t_out++;
  if (t_out >= p)
    {
      t_in = 0;
      pthread_cond_broadcast (&c_out);
    }
  else
    {
      while (t_out < p)
        pthread_cond_wait (&c_out, &m);
    }
  pthread_mutex_unlock(&m);
}


void reduce_sum(int p, double & a)
{
  static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t c_in = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t c_out = PTHREAD_COND_INITIALIZER;
  static int t_in = 0;
  static int t_out = 0;
  static double r = 0;
  if (p <= 1)
    return;
  pthread_mutex_lock(&m);
  if (!(r > 0 || r < 0))
    r = a;
  else
  {
    r += a;
  }
  t_in++;
  if (t_in >= p)
  {
    t_out = 0;
    pthread_cond_broadcast(&c_in);
  }
  else
  {
    while (t_in < p)
      pthread_cond_wait(&c_in, &m);
  }
  if (r > a || r < a)
  {
    a = r;
  }
  t_out++;
  if (t_out >= p)
  {
    t_in = 0;
    r = 0;
    pthread_cond_broadcast(&c_out);
  }
  else
  {
    while (t_out < p)
      pthread_cond_wait(&c_out, &m);
  }
  pthread_mutex_unlock(&m);
}


//     ____________
//    /_/_/_/_/_/_/
//   /_/_/_/_/_/_/
//  /_/_/_/_/
// /_/_/_/_/
//


//   8  9 10 13 14
//  5  6  7 11 12
// 0  1  2
//

void ij2l (int nx, int ny, int i_hole, int j_hole, int i, int j, int &l)
{
  int scr = i_hole + 1;
  if (i <= i_hole)
    l = j * scr + i;
  else
    l = (ny + 1) * scr + (j - j_hole) * (nx - i_hole) + i - i_hole - 1;
}

void l2ij (int nx, int ny, int i_hole, int j_hole, int &i, int &j, int l)
{
  int scr = i_hole + 1;
  if (l < scr * (ny + 1))
    {
      j = l / scr;
      i = l % scr;
    }
  else
    {
      l -= (ny + 1) * scr;
      i = i_hole + 1 + l % (nx - i_hole);
      j = j_hole + l / (nx - i_hole);
    }
}

//   8  9 10 11 12
//  3  4  5  6  7
// 0  1  2
//
/*
void ij2l (int nx, int ny, int i_hole, int j_hole, int i, int j, int &l)
{
  (void)ny;
  int scr = i_hole + 1;
  if (j <= j_hole)
    l = j * scr + i;
  else
    l = (j_hole + 1) * scr + (j - j_hole - 1) * (nx + 1) + i;
}

void l2ij (int nx, int ny, int i_hole, int j_hole, int &i, int &j, int l)
{
  (void)ny;
  int scr = i_hole + 1;
  if (l < scr * (j_hole + 1))
    {
      j = l / scr;
      i = l % scr;
    }
  else
    {
      l -= (j_hole + 1) * scr;
      i = l % (nx + 1);
      j = j_hole + 1 + l / (nx + 1);
    }
}*/
//     ____________
//    /_/_/_/_/_/_/
//   /_/_/_/_/_/_/
//  /_/_/_/_/
// /_/_/_/_/
//

bool inner_point (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)nx;
  if (i < i_hole && i > 0 && j > 0 && j < ny)
    return true;
  if (i >= i_hole && i < nx && j > j_hole && j < ny)
    return true;
  return false;
}

bool down_side_without_angle_point (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)ny;
  if (j == 0 && i > 0 && i < i_hole)
    return true;
  if (j == j_hole && i > i_hole && i < nx)
    return true;
  return false;
}

bool upper_side_without_angle_point (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)i_hole; (void)j_hole;
  if (j == ny && i > 0 && i < nx)
    return true;
  return false;
}

bool left_side_without_angle_point (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)i_hole; (void)j_hole; (void)nx;
  if (i == 0 && j > 0 && j < ny)
    return true;
  return false;
}

bool right_side_without_angle_point (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  if (j > j_hole && i == nx && j < ny)
    return true;    
  else if (j < j_hole && i == i_hole && j > 0)
    return true;
  return false;
}

bool left_down_angle (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)i_hole; (void)j_hole; (void)nx; (void)ny;
  if (i == 0 && j == 0)
    return true;
  return false;
}

bool right_up_angle (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)i_hole; (void)j_hole;
  if (i == nx && j == ny)
    return true;
  return false;
}

bool left_up_angle (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)nx; (void)i_hole; (void)j_hole;
  if (j == ny && i == 0)
    return true;
  return false;
}

bool right_down_angle (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)ny;
  if (j == j_hole && i == nx)
    return true;
  if (j == 0 && i == i_hole)
    return true;
  return false;
}

bool hole_angle (int i, int j, int i_hole, int j_hole, int nx, int ny)
{
  (void)nx; (void)ny;
  if (i == i_hole && j == j_hole)
    return true;
  return false;
}


//CHECKED
//OK
int IA_ij (int nx, int ny, double hx, double hy, int i_hole, int j_hole, int i, int j, int is, int js, int s, int *I, double *A)
{
  int ls, l;
  ij2l (nx, ny, i_hole, j_hole, is, js, ls);
  ij2l (nx, ny, i_hole, j_hole, i, j, l);
  if (I)
    {
      I[s] = ls;
    }
  double Sq = hx * hy;
  if (A)
    {
      if (inner_point (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls) //diag elt
            A[s] = 6 * 1.0 / 12 * Sq;
          else
            A[s] = 2 * 1.0 / 24 * Sq;
          return 0;
        }
      else if (down_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls) //diag elt
            A[s] = 3 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 1)
            A[s] = 1 * 1.0 / 24 * Sq;
          else if (s == 2 || s == 3)
            A[s] = 2 * 1.0 / 24 * Sq;
          else 
            return -1;
          return 0;
        }
      else if (upper_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls) 
            A[s] = 3 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 3)
            A[s] = 1 * 1.0 / 24 * Sq;
          else if (s ==  1 || s == 2)
            A[s] = 2 * 1.0 / 24 * Sq;
          else
            return -1;
          return 0;
        }
      else if (left_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls)
            A[s] = 3 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 3)
            A[s] = 2 * 1.0 / 24 * Sq;
          else if (s == 1 || s == 2)
            A[s] = 1 * 1.0 / 24 * Sq;
          else 
            return -1;
          return 0;
        }
      else if (right_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls)
            A[s] = 3 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 3)
            A[s] = 1 * 1.0 / 24 * Sq;
          else if (s == 1 || s == 2)
            A[s] = 2 * 1.0 / 24 * Sq;
          else 
            return -1;
          return 0;
        }
      else if (left_down_angle (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls)
            A[s] = 2 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 1)
            A[s] = 1 * 1.0 / 24 * Sq;
          else if (s == 2)
            A[s] = 2 * 1.0 / 24 * Sq; //HERE
          else
            return -1;
          return 0;
        }
      else if (right_up_angle (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls)
            A[s] = 2 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 2)
            A[s] = 1 * 1.0 / 24 * Sq;
          else if (s == 1)
            A[s] = 2 * 1.0 / 24 * Sq;
          else 
            return -1;
          return 0;
        }     
      else if (left_up_angle (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls)
            A[s] = 1 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 1)
            A[s] = 1 * 1.0 / 24 * Sq;
          else
            return -1;
          return 0;
        }
      else if (right_down_angle (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls)
            A[s] = 1 * 1.0 / 12 * Sq;
          else if (s == 0 || s == 1)
            A[s] = 1 * 1.0 / 24 * Sq;
          else
            return -1;
          return 0;
        }
      else if (hole_angle (i, j, i_hole, j_hole, nx, ny))
        {
          if (l == ls)
            A[s] = 5 * 1.0 / 12 * Sq;
          else if (s == 2 || s == 3 || s == 4 || s == 5)
            A[s] = 2 * 1.0 / 24 * Sq;
          else if (s == 0 || s == 1)
            A[s] = 1 * 1.0 / 24 * Sq;
          else
            return -1;
          return 0;
        }
    }
  //if (fabs (A[s]) < 1e-14)
  //std::cout << i << " " << j << " " << s << " " << is << " " << js << " " << A[s] << std::endl;
  return -1;
}


#define F(IS, JS, S) (IA_ij(nx, ny, hx, hy, i_hole, j_hole, i, j, (IS), (JS), (S), I, A))
//CHECKED
//OK
int get_off_diag (int nx, int ny, double hx, double hy, int i, int j, int i_hole, int j_hole, int *I, double *A)
{
  //внутр узлы
  if (inner_point (i, j, i_hole, j_hole, nx, ny))
    { 
      if (I && A)
        {
          F (i+1, j, 0); F (i, j-1, 1); F (i-1, j-1, 2); 
          F (i-1, j, 3); F (i, j+1, 4); F (i+1, j+1, 5);
        }
      return 6;
    }
  else if (down_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          F (i+1, j, 0); F (i-1, j, 1); F (i, j+1, 2); F (i+1, j+1, 3); 
        }
      return 4;
    }
  else if (upper_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          F (i+1, j, 0); F (i, j-1, 1); F (i-1, j-1, 2); F (i-1, j, 3); 
        }
      return 4;
    }
  else if (left_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          F (i+1, j, 0); F (i, j-1, 1); F (i, j+1, 2); F (i+1, j+1, 3); 
        }
      return 4;
    }
  else if (right_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          F (i, j-1, 0); F (i-1, j-1, 1); F (i-1, j, 2); F (i, j+1, 3); 
        }
      return 4;
    }
  else if (left_down_angle (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          F (i+1, j, 0); F (i, j+1, 1); F (i+1, j+1, 2);
        }
      return 3;
    }
  else if (right_up_angle (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          //HERE
          //F (i-1, j, 0); F (i-1, j-1, 1); F (i, j-1, 2);
          F (i, j-1, 0); F (i-1, j-1, 1); F (i-1, j, 2);
        }
      return 3;
    }
  else if (left_up_angle (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          F (i+1, j, 0); F (i, j-1, 1);
        }
      return 2;
    }
  else if (right_down_angle (i, j, i_hole, j_hole, nx, ny)) //таких углов два
    {
      if (I && A)
        {
          F (i-1, j, 0); F (i, j+1, 1);
        }
      return 2;
    }
  else if (hole_angle (i, j, i_hole, j_hole, nx, ny))
    {
      if (I && A)
        {
          F (i+1, j, 0); F (i, j-1, 1); F (i-1, j-1, 2); F (i-1, j, 3); F (i, j+1, 4); F(i+1, j+1, 5);
        }
      return 6;
    }
  return -1;
}

#undef F

//CHECKED
//OK
void fill_I (int len, int nx, int ny, double hx, double hy, int i_hole, int j_hole, int *I, double *A, int p, int k)
{
  int i, j, l;
  (void)A;
  int N = len;
  if (k == 0)
    {
      int r = N + 1;
      for (l = 0; l < N; l++)
        {
          l2ij (nx, ny, i_hole, j_hole, i, j, l);
          //int s = get_off_diag (nx, ny, hx, hy, i, j, i_hole, j_hole, I + r, A + r); //число ненулевых элементов
          int s = get_off_diag (nx, ny, hx, hy, i, j, i_hole, j_hole, I, nullptr);
          I[l] = r;
          r += s;
        }
      I[l] = r;
    }
  reduce_sum (p);
}

/*
int fill_IA (int nx, int ny, double hx, double hy, int *I, double *A, int p, int k)
{
  int i, j, l, l1, l2, N = (nx + 1) * (ny + 1);
  l1 = N * k;
  l1 /= p;
  l2 = N * (k + 1);
  l2 /= p;
  int err = 0;
  for (l = l1; l < l2; l++)
    {
      r = I[l];
      s = I[l + 1] - I[l]; //число ненулевых элементов
      l2ij (nx, ny, i, j, l);
      int t = get_off_diag (nx, ny, hx, hy, i, j, I + r, A + r);
      if (t != s)
        {
          err = -1;
          break;
        }
    }
  reduce_sum (p); // (p, &err, 1);
  if (err < 0)
    return -1;
  reduce_sum (p, &len, 1); //сумма длин внедиагольных элементов
  if (I[N] != (N + 1) + len) 
    return -2;
  return 0;
}

*/
//функция считает диагональный элемент
//OK
int get_diag (int nx, int ny, double hx, double hy, int i_hole, int j_hole, int i, int j, double *A)
{
  return IA_ij (nx, ny, hx, hy, i_hole, j_hole, i, j, i, j, 0, nullptr, A);
}


//вариант с заполнением и диагонали
//CHECKED
//OK
int fill_IA (int len, int nx, int ny, int i_hole, int j_hole, double hx, double hy, int *I, double *A, int p, int k)
{
  //fill_I доджен был быть вызван
  int i, j, l, l1, l2, N = len, r, s, t, err = 0, length = 0;
  l1 = N * k; 
  l1 /= p;
  l2 = N * (k + 1);
  l2 /= p;
  for (l = l1; l < l2; l++)
    {
      l2ij (nx, ny, i_hole, j_hole, i, j, l);
      if (get_diag (nx, ny, hx, hy, i_hole, j_hole, i, j, A + l) != 0)
        {
          err = -1;
          break;
        }
      r = I[l]; //начало ненулевого внедиаг элемента
      s = I[l + 1] - I[l]; //число ненулевых внедиаг эл-тов
      //HERE
      t = get_off_diag (nx, ny, hx, hy, i, j, i_hole, j_hole, I + r, A + r);
      
      if (t != s)
       {
         err = -1;
         break;
       }
      len += s;
    }
  //reduce_sum (p, &err, 1);
  reduce_sum (p);
  (void)err;
  //if (err < 0)
  //  return -1;
  //reduce_sum (p, &len, 1);
  if (I[N] != (N + 1) + length)
    return -2;
  return 0;
}


//    _________
//   /|       /
//  / |      /
// /__|_____/
// scr
//

#define F(i, j) (f(x0 + (i)*hx + (j)*scr, y0 + (j)*hy))
double F_ij (int nx, int ny, int i_hole, int j_hole, double scr, double hx, double hy, double i, double j, double x0, double y0, double (*f) (double, double))
{
  double w = (hx * hy) / 192;
  //double sx = hx / 2, sy = hy / 2, sh = scr / 2;
  if (inner_point (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (36 *  F (i, j) 
                + 20 * (  F (i + 0.5, j) + F (i, j - 0.5) + F (i - 0.5, j - 0.5) 
                        + F (i - 0.5, j) + F (i, j + 0.5) + F (i + 0.5, j + 0.5))
                + 4  * (F (i + 0.5, j - 0.5) + F (i - 0.5, j - 1) + F (i - 1, j - 0.5) + 
                        F (i - 0.5, j + 0.5) + F (i + 0.5, j + 1) + F (i + 1, j + 0.5) )
                + 2  * (F (i + 1, j) + F (i, j - 1) + F (i - 1, j - 1) +
                        F (i - 1, j) + F (i, j + 1) + F (i + 1, j + 1)));
    }
  if (down_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (  18 * F (i, j) 
                  + 10 * (F (i + 0.5, j) + F (i - 0.5, j)) 
                  + 20 * (F (i, j + 0.5) + F (i + 0.5, j + 0.5))
                  + 4  * (F (i - 0.5, j + 0.5) + F (i + 0.5, j + 1) + F (i + 1, j + 0.5))
                  + 1  * (F (i + 1, j) + F (i - 1, j))
                  + 2  * (F (i, j + 1) + F (i + 1, j + 1)));
    }
  if (upper_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (  18 * F (i, j) 
                  + 10 * (F (i + 0.5, j) + F (i - 0.5, j))
                  + 20 * (F (i, j - 0.5) + F (i - 0.5, j - 0.5))
                  + 4  * (F (i + 0.5, j - 0.5) + F (i - 0.5, j - 1) + F (i - 1, j - 0.5))
                  + 1 * (F (i + 1, j) + F (i - 1, j))
                  + 2 * (F (i, j - 1) + F (i - 1, j - 1)));
    }
  if (left_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (  18 * F (i, j)
                  + 10 * (F (i, j - 0.5) + F (i, j + 0.5))
                  + 20 * (F (i + 0.5, j + 0.5) + F (i + 0.5, j))
                  +  4 * (F (i + 0.5, j - 0.5) + F (i + 0.5, j + 1) + F (i + 1, j + 0.5))
                  +  1 * (F (i, j - 1) + F (i, j + 1))
                  +  2 * (F (i + 1, j + 1) + F (i + 1, j)));//HERE
    }
  if (right_side_without_angle_point (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (  18 * F (i, j)
                  + 10 * (F (i, j - 0.5) + F (i, j + 0.5))
                  + 20 * (F (i - 0.5, j - 0.5) + F (i - 0.5, j))
                  +  4 * (F (i - 0.5, j - 1) + F (i - 1, j - 0.5) + F (i - 0.5, j + 0.5))
                  +  1 * (F (i, j - 1) + F (i, j + 1))
                  +  2 * (F (i - 1, j - 1) + F (i - 1, j)));
    }
  if (left_down_angle (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (  12 * F (i, j)
                  + 10 * (F (i + 0.5, j) + F (i, j + 0.5))
                  + 20 * F (i + 0.5, j + 0.5) 
                  +  4 * (F (i + 1, j + 0.5) + F (i + 0.5, j + 1))
                  +  1 * (F (i + 1, j) + F (i, j + 1))
                  +  2 * F (i + 1, j + 1));
    }
  if (right_up_angle (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (  12 * F (i, j)
                  + 10 * (F (i - 0.5, j) + F (i, j - 0.5))
                  + 20 * F (i - 0.5, j - 0.5) 
                  +  4 * (F (i - 1, j - 0.5) + F (i - 0.5, j - 1))
                  +  1 * (F (i - 1, j) + F (i, j - 1))
                  +  2 * F (i - 1, j - 1));
    }
  if (left_up_angle (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (   6 * F (i, j)
                  + 10 * (F (i + 0.5, j) + F (i, j - 0.5))
                  +  4 * F (i + 0.5, j - 0.5)
                  +  1 * (F (i + 1, j) + F (i, j - 1)));
    }
  if (right_down_angle (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (   6 * F (i, j)
                  + 10 * (F (i - 0.5, j) + F (i, j + 0.5))
                  +  4 * F (i - 0.5, j + 0.5)
                  +  1 * (F (i - 1, j) + F (i, j + 1))); //HERE WAS MISTAKE
    }
  if (hole_angle (i, j, i_hole, j_hole, nx, ny))
    {
      return w * (  30 * F (i, j)
                  + 20 * (F (i - 0.5, j - 0.5) + F (i - 0.5, j) + F (i, j + 0.5) + F (i + 0.5, j + 0.5))
                  + 10 * (F (i + 0.5, j) + F (i, j - 0.5))
                  +  4 * (F (i + 0.5, j + 1) + F (i + 1, j + 0.5) + F (i - 0.5, j - 1) + F (i - 1, j - 0.5) + F (i - 0.5, j + 0.5))
                  +  2 * (F (i + 1, j + 1) + F (i - 1, j - 1) + F (i - 1, j) + F (i, j + 1))
                  + F (i + 1, j) + F (i, j - 1));
    }
  return 1e-300;
}

#undef F

int fill_b (int len, int nx, int ny, int i_hole, int j_hole, double hx, double hy, double scr, double *b, int p, int k, double x0, double y0, double (*f) (double, double))
{
  //fill_I доджен был быть вызван
  int i, j, l, l1, l2, N = len;// r, s, t, err = 0;
  l1 = N * k; 
  l1 /= p;
  l2 = N * (k + 1);
  l2 /= p;
  for (l = l1; l < l2; l++)
    {
      l2ij (nx, ny, i_hole, j_hole, i, j, l);
      b[l] = F_ij (nx, ny, i_hole, j_hole, scr, hx, hy, i, j, x0, y0, f);
    }
  reduce_sum (p);
  return 0;
}


/*
void mult_msr_matrix_vector (double *A, int *I, int n, double *x, double *y)
{
  int i, j, l, J;
  for (i = 0; i < n; i++)
    {
      s = A[i] * x[i];
      l = I[i + 1] - I[i];
      J += I[i];
      for (j = 0; j < l; j++)
        s += A[J+j] * x[I[J + j]];
      y[i] = s;
    }
}
*/

void mult_msr_matrix_vector (int n, double *A, int *I, double *x, double *y, int p, int k)
{
  int i, j, l, J, i1, i2;
  double s;
  i1 = n * k;
  i1 /= p;
  i2 = n * (k + 1);
  i2 /= p;
  for (i = i1; i < i2; i++)
    {
      s = A[i] * x[i];
      l = I[i + 1] - I[i];
      J = I[i];
      for (j = 0; j < l; j++)
        s += A[J + j] * x[I[J + j]];
      y[i] = s;
    }
  reduce_sum (p);
}

void thread_rows (int n, int p, int k, int &i1, int &i2)
{
  i1 = n * k;
  i1 /= p;
  i2 = n * (k + 1);
  i2 /= p;
}


double scalar_product (int n, double *x, double *y, int p, int k)
{
  int i, i1, i2;
  double s = 0;
  thread_rows (n, p, k, i1, i2);
  for (i = i1; i < i2; i++)
    s += x[i] * y[i];
  reduce_sum (p, s);
  //сдлеать сумму по потокам
  return s;
}

void mult_subvector (int n, double *x, double *y, double t, int p, int k)
{
  int i, i1, i2;
  thread_rows (n, p, k, i1, i2);
  for (i = i1; i < i2; i++)
    x[i] -= t * y[i];
  reduce_sum (p);
}


//проверить
void apply_preconditioner_msr_matrix (int n, double *A, int *I, double *v, double *r, int p, int k)
{
  (void)I;
  int i, i1, i2;
  thread_rows (n, p, k, i1, i2);
  for (i = i1; i < i2; i++)
    {
      v[i] = r[i] / A[i];
    }
  reduce_sum (p);
}


int minimal_error_msr_matrix (int n, double *A, int *I, double *b, double *x, double *r, double *u, double *v, double eps, int maxit, int p, int k)
{
  double prec, b_norm;
  int it;
  b_norm = scalar_product (n, b, b, p, k);
  prec = b_norm * eps * eps; //b_norm * b_norm * eps * eps;
  mult_msr_matrix_vector (n, A, I, x, r, p, k);
  mult_subvector (n, r, b, 1, p, k);
  for (it = 0; it < maxit; it++)
    {
      apply_preconditioner_msr_matrix (n, A, I, v, r, p, k);
      mult_msr_matrix_vector (n, A, I, v, u, p, k);
      double c1 = scalar_product (n, u, r, p, k);
      double c2 = scalar_product (n, u, u, p, k);
      if (fabs (c1) < prec || c2 < prec)
        break;
      double tau = c1 / c2;
      mult_subvector (n, x, v, tau, p, k);
      mult_subvector (n, r, u, tau, p, k);
      /*
      //apply_preconditioner_msr_matrix (n, A, I, x, r, p, k);
      mult_msr_matrix_vector (n, A, I, r, u, p, k);
      double c1 = scalar_product (n, r, r, p, k);
      double c2 = scalar_product (n, u, r, p, k);
      if (c1 < prec || c2 < prec * b_norm)
        break;
      double tau = c1 / c2;
      mult_subvector (n, x, r, tau, p, k);
      mult_subvector (n, r, u, tau, p, k);
      */
    }
  if (it >= maxit)
    return -1;
  return it;
}

int minimal_error_msr_matrix_full (int n, double *A, int *I, double *b, double *x, double *r, double *u, double *v, double eps, int maxit, int max_steps, int p, int k)
{
  int step;
  int its = 0;
  for (step = 0; step < max_steps; step++)
    {
      int ret = minimal_error_msr_matrix (n, A, I, b, x, r, u, v, eps, maxit, p, k);
      its += ret;
      //HERE
      if (ret >= 0)
        break;
    }
  if (step == max_steps)
    return -1;
  return its;
}


//     ____________
//    /_/_/_/_/_/_/
//   /_/_/_/_/_/_/
//  /_/_/_/_/
// /_/_/_/_/
//

int get_len_msr_offdiag (int nx, int ny, int i_hole, int j_hole, int p, int k)
{
  (void)p; (void)k;
  int len =   6 * ((ny - 1) * (i_hole - 1) + (ny - j_hole - 1) * (nx - i_hole))
            + 4 * (nx - 1 + nx - 2 + ny - 1 + ny - 2)
            + 3 * 2
            + 2 * 3
            + 6 * 1;
  return len; 
}

int get_len_msr_diag (int nx, int ny, int i_hole, int j_hole)
{
 return (nx + 1) * (ny + 1) - (nx - i_hole) * j_hole;
}

void check(int n, int *I, double *A)
{
  double *arr = new double[n*n];
  memset (arr, 0, n * n * sizeof(double));
  for (int i = 0; i < n; i++)
    {
      arr[i*n + i] = A[i];
      int l = I[i+1] - I[i];
      int ind = I[i];
      for (int j = 0; j < l; j++)
      {
        //std::cout << i << " " << I[ind+j] << " " << A[ind+j] << std::endl; 
        arr[i*n + I[ind+j]] = A[ind+j];
      }
    }
  bool check = true;
  for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
        {
          if (fabs (arr[i*n+j] - arr[j*n+i]) > 1e-7)
            {
              //std::cout << i << " " << j << " " << arr[i*n+j] << " " << arr[j*n+i] << std::endl;
              check = false;
            }
        }
    }
  (void)check;
  //if (check)
    //std::cout << "OK\n";
  //else
    //std::cout << "AAAAA\n";
  delete [] arr;
}

void solve (thread_info *thr)
{
  int nx = thr->nx;
  int ny = thr->ny;
  int i_hole = thr->i_hole;
  int j_hole = thr->j_hole;
  double scr = thr->scr;
  double hx = thr->hx;
  double hy = thr->hy;
  int p = thr->p;
  int k = thr->k;
  double *A = thr->A;
  double *b = thr->b;
  double *x = thr->x;
  int *I = thr->I;
  double eps = thr->eps;
  double *r = thr->r;
  double *u = thr->u;
  double *v = thr->v;
  double x0 = thr->x0;
  double y0 = thr->y0;
  int maxit = 220;
  int max_steps = 40;
  int len = thr->len;
  reduce_sum (p);
  
  fill_I (len, nx, ny, hx, hy, i_hole, j_hole, I, A, p, k);
  
  reduce_sum (p);  
  
  fill_IA (len, nx, ny, i_hole, j_hole, hx, hy, I, A, p, k);
 
  //check (len, I, A);
  reduce_sum (p);
  
  fill_b (len, nx, ny, i_hole, j_hole, hx, hy, scr, b, p, k, x0, y0, thr->F);
  
  reduce_sum (p);
  
  int its = minimal_error_msr_matrix_full (len, A, I, b, x, r, u, v, eps, maxit, max_steps, p, k);

  static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock (&mtx);
  printf ("THREAD %d : %d iterations\n", k, its);
  pthread_mutex_unlock (&mtx);
  //for (int i = 0; i < len; i++)
  //  {
  //    std::cout << x[i] << std::endl;    
  //  }
  
  
  /*
  for (int q = 0; q < len; q++)
    {
      int l = q;
      int i,j;
      l2ij(nx,ny,i_hole,j_hole,i,j,l);
      ij2l(nx,ny,i_hole,j_hole,i,j,l);
      std::cout << i << " " << j << " " << q << " " << l << std::endl;
    }
  */
  
  reduce_sum (p);
}


