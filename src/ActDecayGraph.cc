// Class for defining a graph of yield versus time

#include "Activia/ActDecayGraph.hh"
#include "Activia/ActGraphPoint.hh"

#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;

ActDecayGraph::ActDecayGraph(const char* name) : ActAbsGraph(name)
{
  // Constructor
  _xAxis = "Time"; _xUnits = "days";
  _yAxes.clear(); _yUnits.clear();
  _yAxes.push_back("Rate");
  _yUnits.push_back("per kg per day");
}

ActDecayGraph::~ActDecayGraph() 
{
  // Destructor
}

void ActDecayGraph::addPoint(double time, double rate) {

  // Add a point to the graph
  ActGraphPoint point(time, rate);
  _points.push_back(point);

  // Update the energy range
  if (time < _minX) {
    _minX = time;
  } else if (time > _maxX) {
    _maxX = time;
  }

  // Sort the vector of points according to time
  std::sort(_points.begin(), _points.end());

}
