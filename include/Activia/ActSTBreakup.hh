#ifndef ACT_ST_BREAKUP_HH
#define ACT_ST_BREAKUP_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate the breakup of nuclei using Silberberg-Tsao formulae
///
/// This class calculates the cross-section for region "B", pg 351, ST'73 II

class ActSTBreakup : public ActAbsXSecModel {

 public:

  /// Construct the breakup model for region B, applying any updates.
  ActSTBreakup(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTBreakup();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
