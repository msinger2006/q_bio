/*************************************************************/
//
// Main program to solve the Plethora Technical Challenge.
//
// Mike Singer
// msinger2006@gmail.com
// (c) 2016
//
/*************************************************************/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <limits>
#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <cstddef>

#include <json/json.h>

#include "Vertex.h"
#include "Edge.h"

using namespace std;
using namespace Json;

// parse json file and store in data structures
void parseProfile(string filename, vector<Edge*> &edges) {

  char* filenameChar = new char[filename.length()+1];
  strcpy(filenameChar, filename.c_str());

  Value root;
  Reader reader;

  ifstream profileStream(filenameChar);
  bool parsingSuccessful = reader.parse(profileStream, root);
  if(!parsingSuccessful) {
    cerr << "Failed to parse profile: " << filename << endl;
    cerr << reader.getFormattedErrorMessages() << endl;
    exit(1);
  }

  // get vertices
  map<int,Vertex*> vertices;
  Vertex *v;
  const Json::Value verts = root["Vertices"];
  for(int i = 0; i < verts.size(); i++) {
    int id = atoi((verts[i].get("id","vertex without id").asString()).c_str());

    const Json::Value coords = verts[i].get("Position","vertex without pos");
    double x = atof((coords.get("X","no x-coord").asString()).c_str());
    double y = atof((coords.get("Y","no y-coord").asString()).c_str());

    v = new Vertex(id,x,y);
    vertices[id] = v;
  }
  delete v;

  // get edges
  const Json::Value edge = root["Edges"];
  Vertex *vertex1, *vertex2;
  Edge *e;
  for(int i = 0; i < edge.size(); i++) {
    int id = atoi((edge[i].get("id","no id tag").asString()).c_str());
    string typeOfEdge = edge[i].get("Type","no type of edge").asString();

    const Json::Value vertexId = edge[i].get("Vertices","no vertices");
    int v1 = atoi((vertexId[0].asString()).c_str());
    int v2 = atoi((vertexId[1].asString()).c_str());

    vertex1 = vertices[v1];
    vertex2 = vertices[v2];
    if(!typeOfEdge.compare("LineSegment")) {
      e = new Edge(id, typeOfEdge, vertex1, vertex2);
    }
    else if(!typeOfEdge.compare("CircularArc")) {
      const Json::Value centerPoint = edge[i].get("Center","no center point");
      double xCenter =
	atof((centerPoint.get("X","no x-coord of center").asString()).c_str());
      double yCenter =
	atof((centerPoint.get("Y","no y-coord of center").asString()).c_str());
      Vertex *center = new Vertex(xCenter, yCenter);

      const Json::Value clockwiseFrom =
	edge[i].get("ClockwiseFrom","no clockwise from point");
      int clockwiseFromId = atoi((clockwiseFrom.asString()).c_str());
      double xClockwise = vertices[clockwiseFromId]->getX();
      double yClockwise = vertices[clockwiseFromId]->getY();
      Vertex *clockwiseVertex = new Vertex(clockwiseFromId,
					   xClockwise, yClockwise);
      e = new Edge(id, typeOfEdge, vertex1, vertex2, center, clockwiseVertex);
    }
    else {
      cerr << "invalide type of edge: " << typeOfEdge << endl;
      exit(1);
    }
    edges.push_back(e);
  }
  delete vertex1, vertex2, e;
}

// compute material cost
double computeMaterialCost(vector<Edge*> &edges, double materialCost,
			   double padding) {
  double xMin = std::numeric_limits<double>::max();
  double xMax = -xMin;
  double yMin = xMin;
  double yMax = xMax;

  // compute bounding box of profile
  for(int i = 0; i < edges.size(); i++) {
    double xMinEdge, xMaxEdge, yMinEdge, yMaxEdge;
    edges[i]->getBoundingBox(xMinEdge, xMaxEdge, yMinEdge, yMaxEdge);
    xMin = min(xMin, xMinEdge);
    xMax = max(xMax, xMaxEdge);
    yMin = min(yMin, yMinEdge);
    yMax = max(yMax, yMaxEdge);
  }

  double deltaX = abs(xMax - xMin) + padding;
  double deltaY = abs(yMax - yMin) + padding;
  double areaOfMaterial = deltaX * deltaY;

  double totalMaterialCost = materialCost * areaOfMaterial;

  return totalMaterialCost;
}

// compute machine cost
double computeMachineCost(vector<Edge*> &edges, double machineTimeCost,
			  double maximalLaserSpeed) {
  double totalMachineCost = 0.;
  for(int i = 0; i < edges.size(); i++) {
    double length = edges[i]->getLength();

    string typeOfEdge = edges[i]->getType();
    if(!typeOfEdge.compare("LineSegment")) {
      totalMachineCost += length * machineTimeCost / maximalLaserSpeed;
    }
    else if(!typeOfEdge.compare("CircularArc")) {
      double radius = edges[i]->getRadius();
      totalMachineCost += length * machineTimeCost /
	(maximalLaserSpeed*exp(-1/radius));
    }
    else {
      cerr << "invalid type of edge: " << typeOfEdge << endl;
      exit(1);
    }
  }
  return totalMachineCost;
}


int main(int argc, char *argv[]) {

  if(argc < 2) {
    cerr << "usage is: main <json profile>" << endl;
    exit(1);
  }
  string filename(argv[1]);

  // cost factors
  const double padding = 0.1;
  const double materialCost = 0.75;
  const double maximalLaserSpeed = 0.5;
  const double machineTimeCost = 0.07;

  // make sure using a .json file
  size_t dotIndx = filename.find(".json");
  if(dotIndx == std::string::npos) {
    cerr << filename << " is an invalid input file name; must use file with " <<
      ".json extension" << endl;
    exit(1);
  }

  // parse json profile and store edges in a vector
  vector<Edge*> edges;
  parseProfile(filename, edges);

  // compute material costs
  double totalMaterialCost = computeMaterialCost(edges, materialCost, padding);

  // compute machine cost
  double totalMachineCost = computeMachineCost(edges, machineTimeCost,
					       maximalLaserSpeed);

  // compute total cost and print to screen
  double totalCost = totalMaterialCost + totalMachineCost;
  totalCost = round(100*totalCost)/100.;
  printf("Quote for %s = $%0.2f\n",
	 filename.substr(0,dotIndx).c_str(), totalCost);

  return 0;

}
