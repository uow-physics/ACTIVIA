#ifndef ACT_TARGET_NUCLIDE_HH
#define ACT_TARGET_NUCLIDE_HH

#include "Activia/ActNuclide.hh"

/// \brief Class to represent target nuclides.
///
/// A target nuclide represents an isotope in the target.
/// It is represented as a ActNuclide with additional information
/// such as the relative abundance fraction as well as other
/// calculated variables like mean values of the atomic mass
/// over all isotopes in the target and cross-section threshold energies.

class ActTargetNuclide : public ActNuclide {

 public:

  /// Construct a target nuclide given atomic and mass numbers,
  /// the halfLife (in days), and the relative abundance of the
  /// isotope (between 0 and 1)
  ActTargetNuclide(int Z, double A, double halfLife = 0.0, 
		   double fraction = 1.0);

  /// Construct a target nuclide given atomic and mass numbers,
  /// the halfLife (in days), and the relative abundance of the
  /// isotope (between 0 and 1). Also let the isotope know the mean and median
  /// values of A over all target isotopes and the energy E_0,
  /// above which cross-sections are independent of energy.
  /// These latter quantities are required for cross-section formulae.
  ActTargetNuclide(int Z, double A, double halfLife,
		   double fraction, double meanA, double medianA,
		   double EZero);
  virtual ~ActTargetNuclide();

  /// Set the relative abundance fraction for the target isotope (0 to 1).
  void setFraction(double fraction) {_fraction = fraction;}
  /// Let the isotope know about the mean atomic mass number for the target
  void setMeanA(double meanA) {_meanA = meanA;}
  /// Let the isotope know about the median atomic mass number for the target (Abar) 
  void setMedianA(double medianA) {_medianA = medianA;}
  /// Let the isotope know about the energy E_0, above which cross-sections are independent of energy.
  void setEZero(double EZero) {_EZero = EZero;}

  /// Retrieve the relatice abundance fraction of the isotope (0 to 1)
  double getFraction() {return _fraction;}
  /// Retrieve the mean atomic mass number for the target
  double getMeanA() {return _meanA;}
  /// Retrieve the median atomic mass number for the target (Abar)
  double getMedianA() {return _medianA;}
  /// retrieve the energy E_0, above which cross-sections are independent of energy.
  double getEZero() {return _EZero;}

  /// Return the pointer to the target nuclide.
  ActNuclide* getNuclide() {return _nuclide;}

 protected:

 private:

  double _fraction, _meanA, _medianA, _EZero;
  ActNuclide* _nuclide;

};

#endif
