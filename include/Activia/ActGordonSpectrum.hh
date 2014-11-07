#ifndef ACT_GORDON_SPECTRUM_HH
#define ACT_GORDON_SPECTRUM_HH

#include "Activia/ActBeamSpectrum.hh"

/// \brief Definition of the Gordon et al. cosmic ray spectrum parameterisation

class ActGordonSpectrum : public ActBeamSpectrum {

 public:

  /// Construct the cosmic ray spectrum.
  ActGordonSpectrum(const char* name = "GordonSpectrum", int ZBeam = 1, double ABeam = 1.0);
  virtual ~ActGordonSpectrum();

  /// Calculate the flux (cm^-2 s^-1) per unit energy (MeV^-1) 
  /// at the given energy (MeV)
  virtual double fluxdE(double e);

 protected:

 private:

};

#endif
