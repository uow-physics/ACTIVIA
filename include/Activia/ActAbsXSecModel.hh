// Abstract class for cross section empirical models/formulae

#ifndef ACT_ABS_XSEC_MODEL_HH
#define ACT_ABS_XSEC_MODEL_HH

#include "Activia/ActFormulae.hh"
#include <string>

class ActNucleiData;

/// \brief Abstract class to define a model to calculate cross-sections.
///
/// All derived classes must implement the calcCrossSection method that
/// calculates the cross-section for a given set of nuclei data 
/// (target, product) pairs.

class ActAbsXSecModel {

 public:

  /// Constructor specifying the name of the cross-section model and
  /// a debugging flag which the output can use to print out helpful
  /// messages during code development.
  ActAbsXSecModel(std::string name, int debug = 0);
  virtual ~ActAbsXSecModel();

  /// All sub-classes must implement the cross-section calculation.
  /// It takes the ActNucleiData pointer, which stores information
  /// about the target and product pair (Z,A) values, and uses this
  /// to calculate the cross-section. This should set the _sigma double
  /// private data member.
  virtual double calcCrossSection(ActNucleiData* data) = 0;

  /// Return the cross-section value calculated by calcCrossSection.
  inline double getCrossSection() {return _sigma;}

  /// Reset the name of the model
  void resetName(std::string name) {_name = name;}
  /// Set the debugging flag
  void setDebugFlag(int debug) {_debug = debug;}

  /// Return the name of the model
  std::string getName() {return _name;}

 protected:
  
  std::string _name;
  double _sigma;
  ActFormulae _formulae;
  int _debug;

 private:

};

#endif
