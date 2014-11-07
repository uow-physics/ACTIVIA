#ifndef ACT_ST_FISS_SPALLATION_HH
#define ACT_ST_FISS_SPALLATION_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate the fission & spallation of nuclei using Silberberg-Tsao formulae
///
/// This class calculates the cross-section for region "FS", pg 350, ST'73 II

class ActSTFissSpallation : public ActAbsXSecModel {

 public:

  /// Construct the fission & spallation model for region FS, applying any updates.
  ActSTFissSpallation(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTFissSpallation();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
