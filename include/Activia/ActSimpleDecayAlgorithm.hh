// Class for using simple exponential formulae for isotope decay

#ifndef ACT_SIMPLE_DECAY_ALGORITHM_HH
#define ACT_SIMPLE_DECAY_ALGORITHM_HH

#include "Activia/ActAbsDecayAlgorithm.hh"

class ActTarget;
class ActProdNuclideList;
class ActTime;
class ActAbsOutput;

/// \brief Implementation of simple exponential decay for isotopes.
///
/// This class calculates the final yield rate for
/// all target-product isotope pairs, given a specified beam
/// exposure and decay time using the formula:
/// final yield = production rate*(1 - exp(-t_exp lambda))*exp(-t_dec lambda)
/// where lamdba = ln(2)/half-life, t_exp is the beam exposure time and
/// t_dec is the decay (cooling) time. The production rate is obtained from
/// the cross-section results from ActIsotopeProduction.
/// Chained decays are not considered.

class ActSimpleDecayAlgorithm : public ActAbsDecayAlgorithm {

 public:

  /// Construct the exponential decay algorithm given a target, a list
  /// of all possible product isotopes, and the exposure and beam times.
  ActSimpleDecayAlgorithm(ActTarget* target, ActProdNuclideList* prodList, 
			  ActTime* times);
  virtual ~ActSimpleDecayAlgorithm();

  /// Calculate the decay yields. Uses the formula:
  /// final yield = production rate*(1 - exp(-t_exp lambda))*exp(-t_dec lambda)
  /// where lamdba = ln(2)/half-life, t_exp is the beam exposure time and
  /// t_dec is the decay (cooling) time.
  virtual void calculateDecays(ActAbsOutput* output);

  /// A method to write out any output at the start of the program
  void writeOutputPreamble();

 protected:
  
 private:

  /// Calculate the exponential decay factor, exp(-lambda*t)
  double calcExpFactor(double time, double halfLife);

};

#endif
