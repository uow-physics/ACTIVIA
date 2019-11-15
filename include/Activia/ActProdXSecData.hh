// Class for defining the production cross-section data for
// a specific target isotope. It stores a map of ActXSecGraphs
// for each target isotope.

#ifndef ACT_PROD_XSEC_DATA_HH
#define ACT_PROD_XSEC_DATA_HH

#include "Activia/ActXSecGraph.hh"
#include "Activia/ActNuclide.hh"

#include <map>

class ActBeamSpectrum;
class ActProdNuclideList;
class ActAbsXSecAlgorithm;
class ActAbsOutput;
class ActTargetNuclide;

/// \brief Less than operator between two ActNuclide pointers for ActProdXSecData.

struct ActPtrLess {
  bool operator() (const ActNuclide* ptr1, const ActNuclide* ptr2) const {
    return (*ptr1) < (*ptr2);
  }
};

/// \brief Store cross-section data for a target isotope and all product nuclei.
///
/// This class calls the cross-section calculations between a target isotope and all 
/// product isotope pairs, using the specified cross-section algorithm, and writes
/// out the cross-section values and isotope production rates.

class ActProdXSecData {

 public:

  /// Construct the calculations of production cross-sections for a given
  /// target isotope and list of possible product isotopes. Also required
  /// is the input beam spectrum, the cross-section algorithm and the output class.
  /// This class also calculates the production rate for each product isotope.
  ActProdXSecData(ActTargetNuclide* targetIsotope, ActProdNuclideList* prodList,
		  ActBeamSpectrum* inputBeam, ActAbsXSecAlgorithm* algorithm, 
		  ActAbsOutput* output);

  virtual ~ActProdXSecData();

  void calculate();

  /// A typedef to define a map of product isotopes and cross-section graphs (sigma vs energy)
  typedef std::map<ActNuclide*, ActXSecGraph, ActPtrLess> ActProdXSecMap;

  // Accessors
  /// Get the full cross-section data: a map associating the cross-section graphs for
  /// all possible product isotopes, given the target isotope specified in the constructor.
  ActProdXSecMap getXSecData() {return _xSecData;}

  /// Get the target isotope
  ActTargetNuclide* getTargetNuclide() {return _targetIsotope;}
  /// Get the list of product isotopes
  ActProdNuclideList* getProdNuclideList() {return _prodList;}
  /// Get the input beam spectrum
  ActBeamSpectrum* getInputBeam() {return _inputBeam;}
  /// Get the cross-section algorithm
  ActAbsXSecAlgorithm* getXSecAlgorithm() {return _algorithm;}

 protected:
  
 private:

  ActTargetNuclide* _targetIsotope;
  ActProdNuclideList* _prodList;
  ActBeamSpectrum* _inputBeam;
  ActAbsXSecAlgorithm* _algorithm;

  // Store XSecGraphs for all product nuclei as well as side branches
  ActProdXSecMap _xSecData;
  ActAbsOutput* _output;

};

#endif
