// Class for defining a graph of cross-section versus energy

#include "Activia/ActXSecGraph.hh"
#include "Activia/ActConstants.hh"
#include "Activia/ActGraphPoint.hh"

#include <algorithm>
#include <cmath>

ActXSecGraph::ActXSecGraph(const char* name) : ActAbsGraph(name)
{
  // Constructor
  _xAxis = "Energy"; _xUnits = "MeV";
  _yAxes.clear(); _yUnits.clear();
  _yAxes.push_back("Sigma"); _yUnits.push_back("mb");
  _yAxes.push_back("ProdRate"); _yUnits.push_back("per kg per day");
}

ActXSecGraph::~ActXSecGraph() 
{
  // Destructor
}

void ActXSecGraph::addPoint(double energy, double sigma, double prodRate) {

  // Add a point to the graph
  ActGraphPoint point(energy, sigma, prodRate);
  _points.push_back(point);

  // Update the energy range
  if (energy < _minX) {
    _minX = energy;
  } else if (energy > _maxX) {
    _maxX = energy;
  }

  // Sort the vector of points according to energy
  std::sort(_points.begin(), _points.end());

}

double ActXSecGraph::getTotalSigma() {

  int nPoints = this->numberOfPoints();
  int i;
  double totalSigma(0.0);

  for (i = 0; i < nPoints; i++) {
    ActGraphPoint point = _points[i];
    totalSigma += point.getY1();
  }

  if (totalSigma < ActConstants::eps) {totalSigma = 0.0;}

  return totalSigma;

}

double ActXSecGraph::getTotalProdRate() {

  int nPoints = this->numberOfPoints();
  int i;
  double totalProdRate(0.0);

  for (i = 0; i < nPoints; i++) {
    ActGraphPoint point = _points[i];
    totalProdRate += point.getY2();
  }

  if (totalProdRate < ActConstants::eps) {totalProdRate = 0.0;}

  return totalProdRate;

}

double ActXSecGraph::calcSigma(double energy) {

  // Get the sigma value based on the energy. Do a linear
  // interpolation if required.
  double sigma(0.0);

  // Check if we have a valid energy value
  if (energy < _minX || energy > _maxX) {return 0.0;}
  
  // Now loop through the (energy-ordered) vectors to find the neighbouring
  // bins on either side of the specified energy to perform a linear
  // interpolation of the cross-section.
  
  double E1(0.0), E2(0.0), sigma1(0.0), sigma2(0.0);
  
  int nPoints = (int) _points.size();
  int iE;

  for (iE = 0; iE < nPoints; iE++) {
    
    ActGraphPoint point = _points[iE];

    double EVal = point.getX();
    double SVal = point.getY();

    double dE = std::fabs(EVal - energy);

    if (EVal < energy || dE < 1e-6) {

      E1 = EVal; sigma1 = SVal;
      E2 = E1; sigma2 = sigma1;

      // We have an "exact" energy match. Stop the loop.
      if (dE < 1e-6) {break;}
      
    } else if (EVal > energy) {
      
      E2 = EVal; sigma2 = SVal;
      // We have got both E1 and E2 now. Stop the loop.
      break;
      
    }
  }

  // Do the linear interpolation to get the sigma value.
  double deltaE = E2 - E1;

  if (std::fabs(deltaE) < 1e-6) {
    // E1 and E2 are the same. Return the sigma1 value (=sigma2).
    sigma = sigma1;
  } else {
    // Do linear interpolation based on the distance between the
    // energy point from the two adjacent "bins"
    double grad = (sigma2 - sigma1)/deltaE;
    double dE1 = energy - E1;
    sigma = grad*dE1 + sigma1;

  }

  return sigma;

}
