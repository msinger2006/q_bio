/*************************************************************/
//
// Vertex class
//
// Mike Singer
// msinger2006@gmail.com
// (c) 2016
//
/*************************************************************/

#ifndef Vertex_H
#define Vertex_H

class Vertex {
 private:
  int id;
  double x, y;

 public:
  Vertex(double x, double y);
  Vertex(int id, double x, double y);
  int getId() const;
  double getX() const;
  double getY() const;
  ~Vertex() { };

};

#endif
