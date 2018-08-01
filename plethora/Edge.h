/*************************************************************/
//
// Edge class
//
// Mike Singer
// msinger2006@gmail.com
// (c) 2016
//
/*************************************************************/

#ifndef Edge_H
#define Edge_H

#include <stdlib.h>
#include <iostream>
#include <string>
#include <cmath>
#include <limits>

#include "Vertex.h"

using namespace std;

static const double eps = std::numeric_limits<double>::min();

class Edge {
 private:
  int id;
  string type;
  Vertex *v1, *v2;
  Vertex *center;
  Vertex *clockwiseFrom;

 public:
  Edge(int id, string type, Vertex *v1, Vertex *v2);
  Edge(int id, string type, Vertex *v1, Vertex *v2, Vertex *center,
       Vertex *clockwiseFrom);
  string getType() const;
  Vertex* getV1() const;
  Vertex* getV2() const;
  double getLength() const;
  double getRadius() const;
  void getBoundingBox(double &xMin, double &xMax,
		      double &yMin, double &yMax) const;
  ~Edge();
};

#endif
