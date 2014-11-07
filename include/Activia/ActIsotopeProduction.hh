#ifndef ACT_ISOTOPE_PRODUCTION_HH
#define ACT_ISOTOPE_PRODUCTION_HH

class ActAbsInput;
class ActAbsOutput;

#include <fstream>

/// \brief Calculates the cross-sections for all target-product isotope pairs.

class ActIsotopeProduction {

 public:

  ActIsotopeProduction(ActAbsInput* inputData, ActAbsOutput* outputData);
  virtual ~ActIsotopeProduction();  

  /// Calculate the cross-sections for all target-product isotope pairs.
  void calcCrossSections();

 protected:
  
 private:

  ActAbsInput* _inputData;
  ActAbsOutput* _outputData;

};

#endif
