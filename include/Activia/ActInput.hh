#ifndef ACT_INPUT_HH
#define ACT_INPUT_HH

#include "Activia/ActAbsInput.hh"
#include "Activia/ActOutputSelection.hh"

#include <iostream>
#include <fstream>
#include <string>

/// \brief Define the inputs (target, products, spectrum, algorithms) for the code

class ActInput : public ActAbsInput {

 public:

  ActInput();
  ActInput(ActOutputSelection* outputSelection);
  virtual ~ActInput();

  /// Set-up the input
  void setUp();

  /// Define the target isotopes
  virtual void defineTarget();
  /// Specify the calculation mode
  virtual void defineCalcMode();
  /// Define the product isotopes
  virtual void defineNuclides();
  /// Define the input beam spectrum
  virtual void defineSpectrum();
  /// Specify the cross-section algorithm
  virtual void specifyXSecAlgorithm();
  /// Define the exposure and decay times for radioactive decay yield calculations
  virtual void defineTime();
  /// Specify the decay yield calculation algorithm
  virtual void specifyDecayAlgorithm();
  /// Define the output file names, format and level of detail.
  virtual void specifyOutput();

  /// Print out the available calculation options.
  virtual void printOptions(std::ofstream& stream);

 protected:

 private:

  std::string _prodDataFile;
  void printIntro();

};

#endif
