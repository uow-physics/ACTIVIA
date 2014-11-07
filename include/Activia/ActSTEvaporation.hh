// Class to calculate light evaporation products, eqns 8-11, paper II

#ifndef ACT_ST_EVAP_HH
#define ACT_ST_EVAP_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate the evaporation of nuclei using Silberberg-Tsao formulae.
///
/// This class calculates the cross-section for light evaporation products, Eqns 8-11, ST'73 II.

class ActSTEvaporation : public ActAbsXSecModel {

 public:

  /// Construct the light evaporation model for region epsilon, applying any updates.
  ActSTEvaporation(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTEvaporation();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
