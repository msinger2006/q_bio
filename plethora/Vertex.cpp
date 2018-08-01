/*************************************************************/
//
// Vertex class
//
// Mike Singer
// msinger2006@gmail.com
// (c) 2016
//
/*************************************************************/

#include "Vertex.h"

// center point of circular arc; no real vertex id
Vertex::Vertex(double x, double y) {
  this->id = -999;
  this->x = x;
  this->y = y;
}

Vertex::Vertex(int id, double x, double y) {
  this->id = id;
  this->x = x;
  this->y = y;
}

int Vertex::getId() const {
  return id;
}

double Vertex::getX() const {
  return x;
}

double Vertex::getY() const {
  return y;
}
