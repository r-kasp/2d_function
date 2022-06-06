#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class thread_info
{
public:
  int k = 0;
  int p = 0;
  double *A = nullptr;
  double *b = nullptr;
  int *I = nullptr;
  double *x = nullptr;
  double *r = nullptr;
  double *u = nullptr;
  double *v = nullptr;
  double (*F) (double, double);
  int i_hole = 0, j_hole = 0;
  double eps;
  int nx;
  int ny;
  double scr;
  double hx;
  double hy;
  double x0;
  double y0;
  int len;
};

//void solve (int nx, int ny, int i_hole, int j_hole, double scr, double hx, double hy, thread_info &thr, double *x);
void solve (thread_info *thr);
void ij2l (int nx, int ny, int i_hole, int j_hole, int i, int j, int &l);
void l2ij (int nx, int ny, int i_hole, int j_hole, int &i, int &j, int l);
int get_len_msr_offdiag (int nx, int ny, int i_hole, int j_hole, int p, int k);
int get_len_msr_diag (int nx, int ny, int i_hole, int j_hole);

#endif
