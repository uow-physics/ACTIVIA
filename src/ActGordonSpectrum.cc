// Class to define the cosmic ray spectrum according to the parameterisation from Gordon et al.
#include "Activia/ActGordonSpectrum.hh"

#include <cmath>

ActGordonSpectrum::ActGordonSpectrum(const char* name, int ZBeam, double ABeam) : ActBeamSpectrum(name, ZBeam, ABeam)
{
}

ActGordonSpectrum::~ActGordonSpectrum() 
{
  // Destructor
}

double ActGordonSpectrum::fluxdE(double e) {

  // Calculate the f(flux)/dE spectrum.
  // Flux in cm^-2 s^-1, E in MeV.

  // Cosmic ray spectrum from Gordon et al: IEEE Nucl. Sci, Vol.51, No.6, 3427 (2004)  
  double beta1(0.3500), beta2(0.4106);
  double gamma1(2.1451), gamma2(-0.6670);
  double c1(1.006e-6), c2(1.011e-3);

  double cosspc(0.0);

  if (e > 1.0e-6) {

    double logE = log(e);
    double logESq = logE*logE;

    double power1 = -beta1*logESq + gamma1*logE;
    double power2 = -beta2*logESq + gamma2*logE;

    cosspc = c1*_formulae.expfun(power1);
    cosspc += c2*_formulae.expfun(power2);

  }

  return cosspc;

}
