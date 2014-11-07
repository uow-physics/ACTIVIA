#ifndef ACT_MASS_DEF_HH
#define ACT_MASS_DEF_HH

#include <vector>

/// \brief Estimates the target and product masses for energy threshold calculations.
///
/// It uses the semi-empirical mass formula from Seeger, Nuc. Phys 25, 1, (1961) 
/// as outlined in Enge, Intro. to Nuclear Physics. 
/// It returns the mass excess in MeV/c2 for the given Z,A nuclide.

class ActMassDef {

 public:

  ActMassDef(double z, double a);
  virtual ~ActMassDef();

  /// Converts the mass from Seeger's 16-O mass scale to the 12-C mass scale
  double calcDelta();
  /// Calculate the mass using the Seeger formula.
  double calcMass();

  void setUp();

  double del(double a);

  /// Calculate threshold energy for zt,at + p -> z,a + p + whatever, where
  /// whatever = max permitted alphas, at most one 3-H and at most one 3-He,
  /// plus required additional p,n.      
  /// Also, assume incident particle remains after reaction, so things
  /// like (n,alpha) reactions are not handled. An (n,alpha) reaction
  /// would be calculated as (n,n+3-He).
  double thr(double zt, double at, double mdtgt, double mdprod);

 protected:

 private:

  double _z, _a, _cmass, _omass;
  double _alpha, _beta, _gamma, _eta, _chi;
  int _nmagicn;

  std::vector<double> _ksi, _nu, _phi, _magicn;

};

#endif
