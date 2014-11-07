#ifndef ACT_ST_SIG_UPDATES_HH
#define ACT_ST_SIG_UPDATES_HH

#include "Activia/ActNucleiData.hh"

#include <vector>

/// \brief Update cross-section values using Silberberg-Tsao modifications.
///
/// Class to update the cross-section calculations for specific
/// target and product nuclei. Some corrections need to be applied
/// over several models, hence they are in this separate class that
/// all can use.

class ActSTSigUpdates {

 public:

  /// Construct the cross-section update class.
  ActSTSigUpdates();

  /// Update the cross-section with any relevant updates (from '85 and '98 papers).
  void updateSigma(ActNucleiData& data, double& sigma);

  virtual ~ActSTSigUpdates();

 protected:

 private:

};

#endif
