#ifndef ACT_XTERM_RUN_HH
#define ACT_XTERM_RUN_HH

#include "Activia/ActAbsRun.hh"

/// \brief Run all of the isotope production code via a terminal interface
///
/// All relevent input and output classes as well as the 
/// cross-section and isotope yield calculations will be called here 
/// using the abstract ActAbsRun interface.

class ActXTermRun : public ActAbsRun {

public:

  /// Constructor
  ActXTermRun();

  /// Destructor
  virtual ~ActXTermRun();

  /// Define how the input to the calculations are obtained.
  virtual void defineInput();

protected:

private:

};

#endif
