// Class to define the cosmic ray spectrum
#include "Activia/ActCosmicSpectrum.hh"

#include <cmath>

ActCosmicSpectrum::ActCosmicSpectrum(const char* name, int ZBeam, double ABeam) : ActBeamSpectrum(name, ZBeam, ABeam)
{
}

ActCosmicSpectrum::~ActCosmicSpectrum() 
{
  // Destructor
}

double ActCosmicSpectrum::fluxdE(double e) {

  // Calculate the f(flux)/dE spectrum.
  // Flux in cm^-2 s^-1, E in MeV.

  // Cosmic ray spectrum basically from [Arm73]
  // parameterization from Geh[85]. Latitude corrections are 10
  // percent-ish within temperate latitudes.

  // Neutron spectral shape factors
  double phiz1(-4.7959), phiz2(-5.301), slop1(-0.50515), slop2(-1.7696);
  // Proton enhancement shape factors
  double p2fac(5.0e-7), pfac2sl(-0.76956), pfac2int(-6.301);

  double cosspc(0.0);
  
  if (e >= 10.0 && e <= 100.0) {

    double log10Term = log10(e*0.1);
    cosspc = slop1*log10Term + phiz1;
    cosspc = _formulae.power(10.0, cosspc);
    cosspc += p2fac;

  } else if (e > 100.0) {

    double log10Term = log10(e*0.01);
    cosspc = slop2*log10Term + phiz2;
    cosspc = _formulae.power(10.0, cosspc);

    if (e <= 1000.0) {

      double cosspcpr = log10Term*pfac2sl + pfac2int;
      cosspcpr = _formulae.power(10.0, cosspcpr);
      cosspc += cosspcpr;

    } else {

      cosspc *= 2.0;

    }

  }

  return cosspc;

}
