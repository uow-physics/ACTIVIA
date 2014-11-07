#ifndef ACT_BEAM_SPECTRUM_HH
#define ACT_BEAM_SPECTRUM_HH

#include <string>
#include <vector>

#include "Activia/ActFormulae.hh"

class ActNuclide;

/// \brief Abstract class to define the interface for an input beam spectrum
///
/// This interface must be used to define an input beam with a known flux per 
/// unit energy (cm^-2 s^-1 per E in MeV).

class ActBeamSpectrum {

 public:

  /// Construct an input beam spectrum.
  /// It requires a name describing what the beam is, as well as the 
  /// atomic and mass number of the particles making up the beam. For example,
  /// protons will have ZBeam = 1, ABeam = 1, neutrons will have ZBeam = 0, ABeam = 1.
  ActBeamSpectrum(const char* name, int ZBeam, double ABeam);
  virtual ~ActBeamSpectrum();

  /// All subclasses must implement the flux per unit energy calculation/definition
  virtual double fluxdE(double e) = 0;

  /// Set the energy range: starting energy, end energy, and energy bin width (all in MeV)
  void setEnergies(double EStart, double EEnd, double dE);

  /// Get the starting energy (MeV)
  double getEStart() {return _EStart;}
  /// Get the end energy (MeV)
  double getEEnd() {return _EEnd;}
  /// Get the energy bin width (MeV)
  double getdE() {return _dE;}
  /// Get the number of energy bins
  int getnE() {return _nE;}

  /// Get the atomic number of the beam particles
  int getZ() {return _ZBeam;}
  /// Get the atomic mass of the beam particles
  double getA() {return _ABeam;}

  /// Set the name of the beam spectrum
  void setName(const char* name) {_name = name;}
  /// Get the name of the beam spectrum
  std::string getName() {return _name;}

  /// Return the pointer to the beam nuclide
  ActNuclide* getNuclide() {return _beamNuclide;}

 protected:

  ActFormulae _formulae;
  double _EStart, _EEnd, _dE;
  int _nE;
  std::string _name;

  int _ZBeam;
  double _ABeam;
  ActNuclide* _beamNuclide;

 private:

};

#endif
