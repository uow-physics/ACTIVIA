#ifndef ACT_TARGET_HH
#define ACT_TARGET_HH

class ActProdNuclideList;
class ActProdXSecData;
class ActBeamSpectrum;
class ActAbsXSecAlgorithm;
class ActTargetNuclide;
class ActAbsOutput;

#include <vector>

/// \brief Class to define the list of isotopes for a specific element (Z value)
///
/// A target can only have one Z value but many A values (isotopes), each with
/// a specified relative abundance fraction, f (0 to 1).

class ActTarget {

 public:

  ActTarget(int Z);
  virtual ~ActTarget();

  // Modifiers
  /// Add an isotope with the given abundance fraction
  void addIsotope(double A, double fraction = 1.0, double halfLife = 0.0);

  void updateIsotopes();

  /// Calculate cross-sections for list of product nuclei, given
  /// input beam spectrum, for each of the target isotopes.
  /// Each element in the vector is the production cross-section data
  /// for the given target isotope and input beam.
  void calcXSections(ActProdNuclideList* prodList,
		     ActBeamSpectrum* inputBeam,
		     ActAbsXSecAlgorithm* algorithm);

  /// Output a summary table of the cross-sections of all of the products
  /// by summing over the target isotopes, weighted by the abundance fractions.
  /// This is called by calcXSections if the output object is set.
  void outputXSecSummary(ActProdNuclideList* prodList);

  void clearProdXSecData();

  /// Set the output class for writing out information
  void setOutput(ActAbsOutput* output) {_output = output;}

  /// Get all of the production cross-section data. Each element in the vector
  /// is the full cross-section data for each target isotope over all available
  /// product isotopes.
  std::vector<ActProdXSecData*> getXSections() {return _xSections;}

  // Accessors
  /// Get the atomic mass for the target isotope, index
  double getAtomicMass(int index);
  /// Get the relative abudance fraction for the target isotope, index (between 0 and 1).
  double getFraction(int index);
  /// Get the relative abundance fraction for the given mass number.
  /// A value needs to be within 1e-6 of the stored value to get a match.
  double getFraction(double A);

  /// Get the integer atomic number of the target
  int getZ() {return _Z;}
  /// Get the double atomic number of the target
  double getfZ() {return _fZ;}
  /// Get the total number of isotopes (same Z, different A) in the target
  int getNIsotopes() {return _nIsotopes;}

  /// Get the isotope specified by the index (0 to _nIsotopes-1)
  ActTargetNuclide* getIsotope(int index);

  /// Get all of the target isotopes.
  std::vector<ActTargetNuclide*> getIsotopes() {return _isotopes;}

  /// Get the median value of the atomic mass over all target isotopes
  double getMedianA() {return _medianA;}
  /// Get the mean value of the atomic mass over all target isotopes
  double getMeanA() {return _meanA;}

  /// Get the input beam spectrum pointer
  ActBeamSpectrum* getInputBeam() {return _inputBeam;}

  // Calculations
  /// Calculate the median value of the atomic mass over all target isotopes.
  /// This is equal to the Abar quatity used in Silberberg-Tsao formulae.
  void calcMedianA();
  /// Calculate the mean value of the atomic mass over all target isotopes.
  /// This is the sum of f_i*A_i over all isotopes i.
  void calcMeanA();

 protected:

 private:

  int _Z;
  double _fZ;

  std::vector<ActTargetNuclide*> _isotopes;

  int _nIsotopes;
  double _meanA, _medianA;

  std::vector<ActProdXSecData*> _xSections;

  ActAbsOutput* _output;
  ActBeamSpectrum* _inputBeam;

};

#endif
