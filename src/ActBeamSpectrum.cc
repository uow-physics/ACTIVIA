// Abstract class to define the interface for an input beam spectrum
// Defines the flux per unit energy (cm^-2 s^-1 per E).
#include "Activia/ActBeamSpectrum.hh"

#include "Activia/ActNuclide.hh"
#include "Activia/ActNuclideFactory.hh"

ActBeamSpectrum::ActBeamSpectrum(const char* name, int ZBeam, double ABeam) : _formulae(), _EStart(0.0), _EEnd(0.0), 
									      _dE(0.0), _nE(0), _name(name),
									      _ZBeam(ZBeam), _ABeam(ABeam), 
									      _beamNuclide(0)
{
  // Constructor
  _beamNuclide = ActNuclideFactory::getInstance()->getNuclide(ZBeam, ABeam, 0.0);
}

ActBeamSpectrum::~ActBeamSpectrum() 
{
  // Destructor
}

void ActBeamSpectrum::setEnergies(double EStart, double EEnd, double dE) {

  // Set the start and end point energies, as well as the bin width
  _EStart = EStart; _EEnd = EEnd; _dE = dE;

  // Calculate the number of bins
  _nE = 1;
  if (dE > 0) {
    _nE = (int) ((EEnd - EStart)/dE);
    _nE += 1; // include EEnd
  }

}
