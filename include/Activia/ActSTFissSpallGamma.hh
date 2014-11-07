#ifndef ACT_ST_FISS_SPALL_GAMMA_HH
#define ACT_ST_FISS_SPALL_GAMMA_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate the fission & spallation (with gamma factor) of nuclei using Silberberg-Tsao formulae
///
/// This class calculates the cross-section for region "F", Eq 7, ST'73 II.

class ActSTFissSpallGamma : public ActAbsXSecModel {

 public:

  /// Construct the fission model for region FS (with gamma term), applying any updates.
  ActSTFissSpallGamma(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTFissSpallGamma();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
