#ifndef ACT_ABS_INPUT_HH
#define ACT_ABS_INPUT_HH

#include <string>
#include <fstream>

class ActTarget;
class ActProdNuclideList;
class ActTime;
class ActBeamSpectrum;
class ActAbsXSecAlgorithm;
class ActAbsDecayAlgorithm;
class ActOutputSelection;

/// \brief Abstract class for inputing data.
///
/// Any derived classes must define several essential inputs: 
/// decay times, target, calculation mode, isotope nuclides, 
/// input beam spectrum and algorithms for cross-section and 
/// radioactive yield calculations.

class ActAbsInput {

 public:

  ActAbsInput();
  ActAbsInput(ActOutputSelection* outputSelection);
  virtual ~ActAbsInput();

  /// Possible run-time options: single product only, or all available isotopes.
  enum ActInputOpt {SingleProd, AllProducts};

  /// This calls the various methods to define and specify the inputs.
  void getData();

  // All sub-classes must implement the cross-section calculation

  /// Define the target (Z, A, Abar values)
  virtual void defineTarget() = 0;
  /// Define the calculation mode (all yields, single isotopes)
  virtual void defineCalcMode() = 0;

  /// Define the product nuclides we want to find the cross-section and yields for
  virtual void defineNuclides() = 0;
  /// Define the input beam spectrum (e.g. cosmic rays) and energy ranges
  virtual void defineSpectrum() = 0;
  /// Define the algorithm for calculating the cross-sections
  virtual void specifyXSecAlgorithm() = 0;
  /// Define the exposure and decay times for radioactive yield calculations
  virtual void defineTime() = 0;
  /// Define the algorithm for calculating the radioactive decay yields for the product isotopes.
  virtual void specifyDecayAlgorithm() = 0;
  /// Define the output file names, format and level of detail. They can be set by the
  /// input class (user interface), but they are then used by the classes inheriting 
  /// from ActAbsOutput.
  virtual void specifyOutput() = 0;

  // Accessor functions
  /// Get the target
  ActTarget* getTarget() {return _target;}
  /// Get the list of possible radioactive product isotopes
  ActProdNuclideList* getProdNuclideList() {return _prodNuclideList;}
  /// Get the input beam spectrum
  ActBeamSpectrum* getSpectrum() {return _spectrum;}
  /// Get the algorithm used to calculating the cross-sections
  ActAbsXSecAlgorithm* getXSecAlgorithm() {return _xSecAlgorithm;}
  /// Get the exposure and decay times
  ActTime* getTime() {return _time;}
  /// Get the algorithm used to calculate the radioactive decay yields for the product isotopes.
  ActAbsDecayAlgorithm* getDecayAlgorithm() {return _decayAlgorithm;}

  /// Retrieve the output information
  ActOutputSelection* getOutputSelection() {return _outputSelection;}

  /// Specify the output printing options
  virtual void printOptions(std::ofstream& ) {;}
  /// Get the printing option
  std::string getOption() {return _option;}
  /// Get the calculation integer flag defined by defineCalcMode().
  int getCalcInt() {return _calcInt;}

 protected:
  
  ActTarget* _target;
  ActProdNuclideList* _prodNuclideList;
  ActBeamSpectrum* _spectrum;
  ActAbsXSecAlgorithm* _xSecAlgorithm;
  ActTime* _time;
  ActAbsDecayAlgorithm* _decayAlgorithm;
  ActOutputSelection* _outputSelection;

  int _calcInt;
  std::string _option;

 private:

};

#endif
