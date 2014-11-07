#ifndef ACT_ST_FISSION_HH
#define ACT_ST_FISSION_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate the fission of nuclei using Silberberg-Tsao formulae
///
/// This class calculates the cross-section for region "F", pg 346 onwards, ST'73 II

class ActSTFission : public ActAbsXSecModel {

 public:

  /// Construct the fission model for region F, applying any updates.
  ActSTFission(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTFission();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
