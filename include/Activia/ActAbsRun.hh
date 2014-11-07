#ifndef ACT_ABS_RUN_HH
#define ACT_ABS_RUN_HH

class ActAbsOutput;
class ActAbsInput;
class ActOutputSelection;
class ActAbsCalcStatus;

/// \brief Run all of the isotope production code.
///
/// All relevent input and output classes as well as the 
/// cross-section and isotope yield calculations must be called here.

class ActAbsRun {

public:

  /// Constructor
  ActAbsRun();

  /// Destructor
  virtual ~ActAbsRun();

  /// This runs the code: input, cross-section calculations, isotope yields and output.
  void run();

  /// Method to define the input interface (stream or GUI etc..)
  virtual void defineInput() = 0;

  /// Method to perform the calculations and store the output
  void doCalculations();

protected:

  ActAbsInput* _input;
  ActOutputSelection* _outputSelection;
  ActAbsCalcStatus* _calcStatus;

  ActAbsOutput* selectXSecOutput();
  ActAbsOutput* selectDecayOutput();
  
private:

};

#endif
