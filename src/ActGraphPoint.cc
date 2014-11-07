// Class for a graph point. The point has only one x value but
// can have several "y" variables, e.g. x = energy, y = sigma/prodrate.

#include "Activia/ActGraphPoint.hh"

ActGraphPoint::ActGraphPoint()
{
  _x = 0.0; _yValues.clear(); _nYValues = 0;
}

ActGraphPoint::ActGraphPoint(double x, double y)
{
  // Constructor
  _x = x; 
  _yValues.clear(); _yValues.push_back(y);
  _nYValues = 1;
}

ActGraphPoint::ActGraphPoint(double x, double y1, double y2)
{
  // Constructor
  _x = x; 
  _yValues.clear(); _yValues.push_back(y1); _yValues.push_back(y2);
  _nYValues = 2;
}

ActGraphPoint::ActGraphPoint(double x, std::vector<double> yValues) 
{
  // Constructor
  _x = x;
  _yValues.clear();
  _nYValues = (int) yValues.size();
  int i;
  for (i = 0; i < _nYValues; i++) {
    _yValues.push_back(yValues[i]);
  }

}

ActGraphPoint::~ActGraphPoint() 
{
  // Destructor
  _yValues.clear();
}

void ActGraphPoint::setYValue(double y) {
  _yValues.clear(); _yValues.push_back(y);
  _nYValues = 1;
}

void ActGraphPoint::setYValues(double y1, double y2) {
  _yValues.clear(); _yValues.push_back(y1); _yValues.push_back(y2);
  _nYValues = 2;
}

void ActGraphPoint::setYValues(std::vector<double> yValues) {

  _yValues.clear();
  _nYValues = (int) yValues.size();
  int i;
  for (i = 0; i < _nYValues; i++) {
    double y = yValues[i];
    _yValues.push_back(y);
  }

}

void ActGraphPoint::addYValue(double y) {
  if (_nYValues > 0) {_yValues[0] += y;}
}

void ActGraphPoint::addYValues(double y1, double y2) {
  if (_nYValues > 1) {_yValues[0] += y1; _yValues[1] += y2;}
}

void ActGraphPoint::addYValues(std::vector<double> yValues) {

  int nYValues = (int) yValues.size();
  if (nYValues == _nYValues) {

    int i;
    for (i = 0; i < _nYValues; i++) {
      _yValues[i] += yValues[i];
    }

  }

}

double ActGraphPoint::getY2() {
  // Return 2nd y value
  if (_nYValues < 2) {return 0.0;}
  return _yValues[1];
}

bool ActGraphPoint::operator < (const ActGraphPoint& other) const {

  bool lessThan(false);
  if (_x < other._x) {lessThan = true;}
  return lessThan;

}
