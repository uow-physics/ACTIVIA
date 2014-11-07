#ifndef ACT_DECAY_GRAPH_HH
#define ACT_DECAY_GRAPH_HH

#include "Activia/ActAbsGraph.hh"

#include <string>
#include <vector>

/// \brief Class for defining a graph of yield rate versus time

class ActDecayGraph : public ActAbsGraph {

 public:

  ActDecayGraph(const char* name = "decayGraph");
  virtual ~ActDecayGraph();

  // Modifiers
  /// Add a (time, yield rate) point to the graph
  void addPoint(double time, double rate);

 protected:
  
 private:

};

#endif
