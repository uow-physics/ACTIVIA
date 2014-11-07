#ifndef ACT_COSMIC_SPECTRUM_HH
#define ACT_COSMIC_SPECTRUM_HH

#include "Activia/ActBeamSpectrum.hh"

/// \brief Definition of a cosmic ray spectrum

class ActCosmicSpectrum : public ActBeamSpectrum {

 public:

  /// Construct a cosmic ray spectrum.
  ActCosmicSpectrum(const char* name = "CosmicRays", int ZBeam = 1, double ABeam = 1.0);
  virtual ~ActCosmicSpectrum();

  /// Calculate the flux (cm^-2 s^-1) per unit energy (MeV^-1) 
  /// at the given energy (MeV)
  virtual double fluxdE(double e);

 protected:

 private:

};

#endif
