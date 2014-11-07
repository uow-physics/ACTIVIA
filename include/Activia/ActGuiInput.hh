#ifdef ACT_USE_QT

#ifndef ACT_GUI_INPUT_HH
#define ACT_GUI_INPUT_HH

#include "Activia/ActAbsInput.hh"
#include "Activia/ActOutputSelection.hh"
#include "Activia/ActGuiWindow.hh"

#include <iostream>
#include <fstream>
#include <string>

/// \brief Define the inputs (target, products, spectrum, algorithms) for the code via a GUI

class ActGuiInput : public ActAbsInput {

 public:

  /// Default constructor
  ActGuiInput();
  /// Constructor
  ActGuiInput(ActOutputSelection* outputSelection, ActGuiWindow* theGui);
  /// Destructor
  virtual ~ActGuiInput();

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
  virtual void printOptions(std::ofstream&) {;}

 protected:

 private:

  void printIntro() {;}
  ActGuiWindow* _theGui;

};

#endif

#endif

