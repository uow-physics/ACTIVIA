#ifndef ACT_XSEC_GRAPH_HH
#define ACT_XSEC_GRAPH_HH

#include "Activia/ActAbsGraph.hh"

#include <string>
#include <vector>

/// \brief Graph of cross-section (mb) and production rate (per kg per day) versus energy (MeV).

class ActXSecGraph : public ActAbsGraph {

 public:

  /// Construct a graph storing cross-section (mb) and production rate (per kg per day) 
  /// versus energy (MeV).
  ActXSecGraph(const char* name = "xSecGraph");
  virtual ~ActXSecGraph();

  /// Get the total cross-section (mb) summed over all energies
  double getTotalSigma();
  /// Get the total production rate (per kg per day) summed over all energies
  double getTotalProdRate();

  /// Perform a linear interpolation to calculate the cross-section (mb) 
  /// for a given energy (MeV), using the points in the graph. 
  double calcSigma(double energy);

  // Modifiers
  /// Add a cross-section (mb) and production rate (per kg per day) value for a given energy (MeV).
  void addPoint(double energy, double sigma, double prodRate);

 protected:
  
 private:

};

#endif
