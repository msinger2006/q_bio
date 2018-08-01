/*************************************************************/
//
// Edge class
//
// Mike Singer
// msinger2006@gmail.com
// (c) 2016
//
/*************************************************************/

#include "Edge.h"

Edge::Edge(int id, string type, Vertex *v1, Vertex *v2) {
  this->id = id;
  this->type= type;
  this->v1 = v1;
  this->v2 = v2;
}

Edge::Edge(int id, string type, Vertex *v1, Vertex *v2,
	   Vertex *center, Vertex *clockwiseFrom) {
  this->id = id;
  this->type = type;
  this->v1 = v1;
  this->v2 = v2;
  this->center = center;
  this->clockwiseFrom = clockwiseFrom;
}

string Edge:: getType() const {
  return type;
}

Vertex* Edge::getV1() const {
  return v1;
}

Vertex* Edge::getV2() const {
  return v2;
}

double Edge::getLength() const {
  double length = 0;
  double v1x = v1->getX();
  double v1y = v1->getY();
  double v2x = v2->getX();
  double v2y = v2->getY();

  // edge length is dependent on type of edge
  if(!type.compare("LineSegment")) {
    double deltaX = v1x - v2x; 
    double deltaY = v1y - v2y;
    length = sqrt(deltaX*deltaX + deltaY*deltaY);
  }
  else if(!type.compare("CircularArc")) {
    double radius = getRadius();

    // endpoint to center
    double xCenter = center->getX();
    double yCenter = center->getY();

    double x1 = v1x - xCenter;
    double y1 = v1y - yCenter;

    double x2 = v2x - xCenter;
    double y2 = v2y - yCenter;

    // dot product and determinant
    double dotProd = x1*x2 + y1*y2;
    double det = x1*y2 - y1*x2;

    // compute angle between vectors and scale appropriately
    double theta = std::atan2(det, dotProd);
    if(theta > 0) {
      theta = 2.*M_PI - theta;
    }
    theta = abs(theta);

    // length of arc
    length = radius * theta;
  }
  else {
    cout << "invalid type of edge = " << type << endl;
    exit(1);
  }
  return length;
}

// radius for arc edges
double Edge::getRadius() const {
  double deltaX = v1->getX() - center->getX();
  double deltaY = v1->getY() - center->getY();
  double length = sqrt(deltaX*deltaX + deltaY*deltaY);
  return length;
}

// bounding box of edge
void Edge::getBoundingBox(double &xMin, double &xMax,
			  double &yMin, double &yMax) const {
  double v1x = v1->getX();
  double v1y = v1->getY();
  double v2x = v2->getX();
  double v2y = v2->getY();

  xMin = min(v1x, v2x);
  xMax = max(v1x, v2x);
  yMin = min(v1y, v2y);
  yMax = max(v1y, v2y);

  if(!type.compare("CircularArc")) {
    double r = getRadius();

    double xCenter = center->getX();
    double yCenter = center->getY();

    double xClockwisePoint = clockwiseFrom->getX();
    double yClockwisePoint = clockwiseFrom->getY();

    // compute cross product; vector to origin and (d*1) vector to center
    // of arc (d*2)
    double dx1 = -xClockwisePoint;
    double dy1 = -yClockwisePoint;

    double dx2 = xCenter - xClockwisePoint;
    double dy2 = yCenter - yClockwisePoint;

    // special treatment if (xCenter,yCenter) = (0,0); shift off
    // (0,0) a little
    if((abs(xCenter) <= eps) && (abs(yCenter) <= eps)) {
      dy1 += 0.01;
      dy2 -= 0.01;

      dx1 += 0.01;
      dx2 -= 0.01;
    }

    double determinant = dx1*dy2 - dx2*dy1;

    // arc protrudes body when det > 0, so extend bounding box
    // when center point has same x- or y-coordinate as clockwiseFrom
    // point, extrude bounding box in y- or x-direction, respectively;
    // direction of extrusion is dependent on dx2
    // otherwise, find max bounding box
    if(determinant >= 0) {
      if((abs(xCenter - xClockwisePoint) <= eps) && (dx2 >= 0)) {
	xMax = xCenter + r;
      }
      else if((abs(xCenter - xClockwisePoint) <= eps) && (dx2 <= 0)) {
	xMin = xCenter - r;
      }
      else if((abs(yCenter - yClockwisePoint) <= eps) && (dy2 >= 0)) {
	yMax = yCenter + r;
      }
      else if((abs(yCenter - yClockwisePoint) <= eps) && (dy2 <= 0)) {
	yMin = yCenter - r;
      }
      else {
	xMin = min(xMin, xCenter-r);
	xMax = max(xMax, xCenter+r);

	yMin = min(yMin, yCenter-r);
	yMax = max(yMax, yCenter+r);
      }
      
    }
  }

}

Edge::~Edge() {
  delete v1, v2, center, clockwiseFrom;
}

