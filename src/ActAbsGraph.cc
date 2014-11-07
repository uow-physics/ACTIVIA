// Abstract class for defining a graph

#include "Activia/ActAbsGraph.hh"

#include <algorithm>

ActAbsGraph::ActAbsGraph(const char* name) : _name(name)
{
  // Constructor
  this->reset();
}

ActAbsGraph::~ActAbsGraph() 
{
  // Destructor
  _points.clear(); _yAxes.clear(); _yUnits.clear();
}

void ActAbsGraph::reset() {
  _points.clear();
  _minX = 1e10; _maxX = 0.0;
  _xAxis = ""; _xUnits = "";
  _yAxes.clear(); _yUnits.clear();
}

void ActAbsGraph::addPoints(std::vector<ActGraphPoint> points) {

  int nPoints = (int) points.size();
  int i;
  for (i = 0; i < nPoints; i++) {
    ActGraphPoint onePoint = points[i];
    _points.push_back(onePoint);
  }

  // Sort the vector of points according to the x variable (e.g energy or time)
  std::sort(_points.begin(), _points.end());

}

int ActAbsGraph::numberOfPoints() {

  // Return the number of points
  int nPoints = (int) _points.size();
  return nPoints;

}

void ActAbsGraph::setYAxisName(std::string yAxis) {
   _yAxes.clear(); _yAxes.push_back(yAxis);
}

void ActAbsGraph::setYAxesNames(std::string yAxis1, std::string yAxis2) {
   _yAxes.clear(); 
   _yAxes.push_back(yAxis1); _yAxes.push_back(yAxis2);
}

void ActAbsGraph::setYAxesNames(std::vector< std::string > yAxes) {
   _yAxes.clear();
   int nAxes = (int) yAxes.size();
   int i;
   for (i = 0; i < nAxes; i++) {
     _yAxes.push_back(yAxes[i]);
   }
}

void ActAbsGraph::setYAxisUnits(std::string yUnits) {
   _yUnits.clear(); _yUnits.push_back(yUnits);
}

void ActAbsGraph::setYAxesUnits(std::string yUnits1, std::string yUnits2) {
   _yUnits.clear(); 
   _yUnits.push_back(yUnits1); _yUnits.push_back(yUnits2);
}

void ActAbsGraph::setYAxesUnits(std::vector< std::string > yUnits) {
   _yUnits.clear();
   int nUnits = (int) yUnits.size();
   int i;
   for (i = 0; i < nUnits; i++) {
     _yUnits.push_back(yUnits[i]);
   }
}

std::string ActAbsGraph::getY2AxisName() {

  std::string yAxisName("");
  int nYAxes = (int) _yAxes.size();
  if (nYAxes > 1) {
    yAxisName = _yAxes[1];
  }
  return yAxisName;

}

std::string ActAbsGraph::getYAxisName(int iY) {

  std::string name("");
  int nYAxes = (int) _yAxes.size();;
  if (iY >= 0 && iY < nYAxes) {name = _yAxes[iY];}
  
  return name;

}

std::string ActAbsGraph::getY2AxisUnits() {

  std::string yAxisUnits("");
  int nYAxes = (int) _yUnits.size();
  if (nYAxes > 1) {
    yAxisUnits = _yAxes[1];
  }
  return yAxisUnits;

}

std::string ActAbsGraph::getYAxisUnits(int iY) {

  std::string units("");
  int nYAxes = (int) _yUnits.size();
  if (iY >= 0 && iY < nYAxes) {units = _yUnits[iY];}
  
  return units;

}
