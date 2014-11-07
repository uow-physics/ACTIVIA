#ifndef ACT_GRAPH_POINT_HH
#define ACT_GRAPH_POINT_HH

#include <vector>

/// \brief Class for defining a graph point for ActAbsGraphs.
///
/// A graph point can only have one x value but can have several y values.
/// For example, x = energy, y1 = sigma, y2 = yield.

class ActGraphPoint {

 public:

  /// Default constructor
  ActGraphPoint();

  /// Define a (x,y) point
  ActGraphPoint(double x, double y);

  /// Define a point with x and two y values
  ActGraphPoint(double x, double y1, double y2);

  /// Define a point with x and a vector of y values
  ActGraphPoint(double x, std::vector<double> yValues);

  virtual ~ActGraphPoint();

  // Set the x/y values
  /// Set the value of x
  void setX(double x) {_x = x;}
  /// Set the first y value
  void setYValue(double y);
  /// Set the first two y values
  void setYValues(double y1, double y2);
  /// Set the y values with the input vector
  void setYValues(std::vector<double> yValues);

  // Increase/decrease the y values by the specified amount
  /// Increment the first y value by the given amount
  void addYValue(double y);
  /// Increment the first two y values by the given amounts
  void addYValues(double y1, double y2);
  /// Increment the y values by the given vector
  void addYValues(std::vector<double> yValues);

  // Retrieve the x and y values
  /// Get the x value
  double getX() {return _x;}
  /// Get the vector of y values
  std::vector<double> getYValues() {return _yValues;}
  /// Get the first y value
  double getY() {return _yValues[0];}
  /// Get the first y value
  double getY1() {return _yValues[0];}
  /// Get the second y value
  double getY2();

  /// Get the number of y values
  int getnYValues() {return _nYValues;}

  /// Less than operator based only on comparison of the x value.
  /// Use this to sort points according to their order in x only.
  bool operator < (const ActGraphPoint& other) const;

 protected:
  
 private:

  double _x;
  std::vector<double> _yValues;
  int _nYValues;

};

#endif
