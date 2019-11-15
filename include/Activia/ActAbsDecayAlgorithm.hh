#ifndef ACT_ABS_DECAY_ALGORITHM_HH
#define ACT_ABS_DECAY_ALGORITHM_HH

#include "Activia/ActNuclide.hh"
#include "Activia/ActDecayGraph.hh"

#include <map>

class ActTarget;
class ActProdNuclideList;
class ActTime;
class ActAbsOutput;

/// \brief Less than operator between two ActNuclide pointers in ActAbsDecayAlgorithm.

struct ActDecayPtrLess {
  bool operator() (const ActNuclide* ptr1, const ActNuclide* ptr2) const {
    return (*ptr1) < (*ptr2);
  }
};

/// \brief An abstract class that calculates radioactive isotope yields.
///
/// It requires an input target, the isotopes produced, and the decay times.

class ActAbsDecayAlgorithm {

 public:

  /// Constructor requires the target, list of radioactive isotopes produced,
  /// and the decay times (exposure and cooling times).
  ActAbsDecayAlgorithm(ActTarget* target, ActProdNuclideList* prodList, 
		       ActTime* times);

  /// Destructor
  virtual ~ActAbsDecayAlgorithm();

  /// A typedef to represent a map between nuclides and decay graphs
  typedef std::map<ActNuclide*, ActDecayGraph, ActDecayPtrLess> ActDecayMap;

  /// Return the <nuclide, decay graph> map
  ActDecayMap getDecayMap() {return _decayMap;}

  /// A pure virtual function to calculate the radioactive decay yields and
  /// print them to the output file.
  virtual void calculateDecays(ActAbsOutput* output) = 0;

 protected:
  
  ActTarget* _theTarget;
  ActProdNuclideList* _theProdList;
  ActTime* _times;
  ActDecayMap _decayMap;
  ActAbsOutput* _outputData;

 private:

};

#endif
