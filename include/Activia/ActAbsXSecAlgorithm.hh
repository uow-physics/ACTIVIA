#ifndef ACT_ABS_XSEC_ALGORITHM_HH
#define ACT_ABS_XSEC_ALGORITHM_HH

class ActNucleiData;
class ActTargetNuclide;
class ActProdNuclideList;

/// \brief Abstract class for defining algorithms for cross-section calculations

class ActAbsXSecAlgorithm {

 public:

  ActAbsXSecAlgorithm() {_nucleiData = 0;}
  virtual ~ActAbsXSecAlgorithm() {;}

  /// Set the target and product isotope data that can be used in cross-section
  /// algorithms.
  virtual void setNucleiData(ActNucleiData* data) {_nucleiData = data;}

  // These methods must be implemented

  /// All derived classes must implement a way to calculate the cross-sections
  virtual double calcCrossSection() = 0;
  /// All derived classes must define a way to load in any data tables for
  /// looking-up cross-section values
  virtual void loadDataTables(ActTargetNuclide* targetNuclide,
			      ActProdNuclideList* prodList) = 0;

  /// Apply any required selection criteria for the nuclides
  virtual bool passSelection(ActNucleiData* data) = 0;

  /// Apply any energy selection criteria
  virtual bool passESelection(ActNucleiData* data) = 0;

  /// Retrieve nuclei data pointer
  ActNucleiData* getNucleiData() {return _nucleiData;}

 protected:
  
  ActNucleiData* _nucleiData;

 private:

};

#endif
