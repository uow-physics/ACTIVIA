#ifndef ACT_ST_FISS_BREAKUP_HH
#define ACT_ST_FISS_BREAKUP_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate the breakup and fission of nuclei using Silberberg-Tsao formulae
///
/// This class calculates the cross-section for region "BF", pg 351, ST'73 II

class ActSTFissBreakup : public ActAbsXSecModel {

 public:

  /// Construct the fission-breakup model for region BF, applying any updates.
  ActSTFissBreakup(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTFissBreakup();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
