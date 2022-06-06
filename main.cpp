
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QMessageBox>
#include <iostream>
#include "matrix.h"

#include "window.h"


//  A  ____________ F
//    /           /
//   /       D___/ E
//  /________/
// B         C   

void skip_line (FILE *fp)
{
  char c = '\0';
  while (c != '\n')
    {
      if (!fscanf (fp, "%c", &c))
        return;
    }
}

void line_to_point (FILE *fp, Point &A)
{
  double x, y;
  if (!fscanf (fp, "%lf %lf", &x, &y))
    return;
  A.x = x;
  A.y = y;
  A.z = 0;
}

void parse (char *filename, Point &A, Point &B, Point &C, 
                            Point &D, Point &E, Point &F)
{
  FILE *fp = fopen (filename, "r");
  
  skip_line (fp);
  skip_line (fp);
  skip_line (fp);
  line_to_point (fp, A);
  skip_line (fp);
  
  skip_line (fp);
  line_to_point (fp, B);
  skip_line (fp);
  
  skip_line (fp);
  line_to_point (fp, F);
  skip_line (fp);
  
  skip_line (fp);
  line_to_point (fp, D);
  
  //  A  ____________ F
  //    /           /
  //   /       D___/ E
  //  /________/
  // B         C   
  
  Point M;
  double coef_vert = (D.y - B.y) / (A.y - D.y);
  M.SetCoordinates ((F.x + B.x) / 2, (F.y + B.y) / 2, 0);
  M.x = 2 * M.x - A.x;
  M.y = 2 * M.y - A.y;
  Point V (coef_vert * (A.x - B.x), coef_vert * (A.y - B.y), 0);
  
  E.y = D.y; E.x = M.x + V.x; C.z = 0;
  C.y = B.y; C.x = D.x - V.x; C.z = 0;
  
  fclose (fp);
}

int main (int argc, char *argv[])
{
  int nx, ny, func_id, p;
  double eps;
  char *filename;
  
  if (   argc != 7
      || sscanf (argv[2], "%d", &nx) != 1
      || sscanf (argv[3], "%d", &ny) != 1
      || sscanf (argv[4], "%d", &func_id) != 1
      || sscanf (argv[5], "%lf", &eps) != 1
      || sscanf (argv[6], "%d", &p) != 1
      || nx <= 1 
      || ny <= 1)
    {
      printf ("Wrong Input. Right : %s config.txt nx ny k eps p.\n nx and ny should be > 1\n", argv[0]);
      return -1;
    }
  filename = argv[1];

  // P1  ____________ P6
  //    /           /
  //   /       P4__/ P5
  //  /________/
  //P2         P3   
  
  Point P1, P2, P3, P4, P5, P6;
  parse (filename, P1, P2, P3, P4, P5, P6);






  //Рисует после счёта
  QApplication app (argc, argv);

  QMainWindow *window = new QMainWindow;
  QMenuBar *tool_bar = new QMenuBar (window);
  Window *graph_area = new Window (window, filename, func_id, nx, ny, eps, p, P1, P2, P3, P4, P5, P6);
  QAction *action;
    
  action = tool_bar->addAction ("&Change function", graph_area, SLOT (set_func ()));
  action->setShortcut (QString ("0"));
  
  action = tool_bar->addAction ("&Change function", graph_area, SLOT (change_line_up ()));
  action->setShortcut (QString ("1"));
  
  action = tool_bar->addAction ("&Increase zoom", graph_area, SLOT (decrease_zoom ()));
  action->setShortcut (QString ("2"));
  
  action = tool_bar->addAction ("&Decrease zoom", graph_area, SLOT (increase_zoom ()));
  action->setShortcut (QString ("3"));
  
  action = tool_bar->addAction ("&Increase nx ny", graph_area, SLOT (increase_n ()));
  action->setShortcut (QString ("4"));
  
  action = tool_bar->addAction ("&Decrease nx ny", graph_area, SLOT (decrease_n ()));
  action->setShortcut (QString ("5"));
  
  action = tool_bar->addAction ("&Increase vertical angle", graph_area, SLOT (increase_z_coef ()));
  action->setShortcut (QString ("6"));
  
  action = tool_bar->addAction ("&Decrease vertical angle", graph_area, SLOT (decrease_z_coef ()));
  action->setShortcut (QString ("7"));
  
  action = tool_bar->addAction ("&Increase angle", graph_area, SLOT (increase_angle ()));
  action->setShortcut (QString ("8"));
  
  //action = tool_bar->addAction ("&Decrease angle", graph_area, SLOT (decrease_angle ()));
  //action->setShortcut (QString ("9"));
  
  action = tool_bar->addAction ("E&xit", window, SLOT (close ()));
  action->setShortcut (QString ("Ctrl+X"));

  tool_bar->setMaximumHeight (30);

  window->setMenuBar (tool_bar);
  window->setCentralWidget (graph_area);
  window->setWindowTitle ("Graph");

  window->show ();
  app.exec ();
  delete window;
  return 0;
}
