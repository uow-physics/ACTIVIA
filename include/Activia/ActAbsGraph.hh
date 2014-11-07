#ifndef ACT_ABS_GRAPH_HH
#define ACT_ABS_GRAPH_HH

#include "Activia/ActGraphPoint.hh"

#include <string>
#include <vector>

/// \brief Abstract class for defining a graph.

/// Each point in the graph can only have one x value, but many y values.
/// This makes it possible for several variables that depend on one variable 
/// to be stored in the same place, e.g. x = energy, while y = sigma and yield.

class ActAbsGraph {

 public:

  /// Construct a graph with a given name
  ActAbsGraph(const char* name = "");
  virtual ~ActAbsGraph();

  /// Set the name of the x axis
  void setXAxisName(std::string xAxis) {_xAxis = xAxis;}
  /// Set the name of the (first) y axis
  void setYAxisName(std::string yAxis);
  /// Set the names of the first two y axes
  void setYAxesNames(std::string yAxis1, std::string yAxis2);
  /// Set the names of all of the y axes
  void setYAxesNames(std::vector< std::string > yAxes);

  /// Set the unit of the x variable
  void setXAxisUnits(std::string xUnits) {_xUnits = xUnits;}
  /// Set the unit of the (first) y variable
  void setYAxisUnits(std::string yUnits);
  /// Set the units of the first two y variables
  void setYAxesUnits(std::string yUnits1, std::string yUnits2);
  /// Set the units of all of the y variables
  void setYAxesUnits(std::vector< std::string > yUnits);

  // Accessors
  /// Number of points in the graph
  int numberOfPoints();
  /// Get a vector of all of the graph points
  std::vector<ActGraphPoint> getPoints() {return _points;}

  /// Get the minimum x value of the graph
  inline double getMinX() {return _minX;}
  /// Get the maximum x value of the graph
  inline double getMaxX() {return _maxX;}

  /// Get the name of the x axis
  std::string getXAxisName() {return _xAxis;}
  /// Get the name of the first y axis
  std::string getYAxisName() {return _yAxes[0];}
  /// Get the name of the first y axis
  std::string getY1AxisName() {return _yAxes[0];}
  /// Get the name of the second y axis
  std::string getY2AxisName();
  /// Get the names of all of the y axes
  std::vector< std::string > getYAxesNames() {return _yAxes;}
  /// Get the name of a specific y axis
  std::string getYAxisName(int iY);

  /// Get the unit of the x variable
  std::string getXAxisUnits() {return _xUnits;}
  /// Get the unit of the first y variable
  std::string getYAxisUnits() {return _yUnits[0];}
  /// Get the unit of the first y variable
  std::string getY1AxisUnits() {return _yUnits[0];}
  /// Get the unit of the second y variable
  std::string getY2AxisUnits();
  /// Get the units of all of the y variables
  std::vector< std::string >getYAxesUnits() {return _yUnits;}
  /// Get the unit for a specific y variable
  std::string getYAxisUnits(int iY);

  /// Get the number of y variables (axes)
  int getnYAxes() {return (int) _yAxes.size();}

  /// Get the name of the graph.
  std::string getName() {return _name;}

  // Modifiers
  /// Add a vector of points to the graph
  void addPoints(std::vector<ActGraphPoint> points);
  /// Reset the graph. This clears all points, axis names, units and x limits.
  void reset();

 protected:

  std::string _name;
  std::vector<ActGraphPoint> _points;
  double _minX, _maxX;
  std::string _xAxis, _xUnits;
  std::vector< std::string > _yAxes, _yUnits;
  
 private:

};

#endif
