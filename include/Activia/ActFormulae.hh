#ifndef ACT_FORMULAE_HH
#define ACT_FORMULAE_HH

#include <string>

/// \brief Class to calculate formulae found throughout the code.
///
/// Various functions used in many parts of the Silberberg-Tsao cross-section 
/// calculations are placed here.

class ActFormulae {

 public:

  ActFormulae();
  virtual ~ActFormulae();

  /// Calculate the (N/Z)* factor (Eq 4b in ST'73 II)
  double nzstar(double zp, double ap, double zt, double at, double atbar);

  /// Calculate G (Eq 15 in ST'73 II)
  double bigg(double z, double a, double zt, double at, double atbar, double e);

  /// Calculate f_B(E) (Eq 15 in ST'73 II)
  double fbe(double z, double a, double zt, double at, 
	     double atbar, double e, double ezero);

  /// Calculate Omega (see Table 2 in ST'73 I)
  double calcOmega(double zp, double ap, std::string formula);
  /// Calculate xi (see Table 2 in ST '85)
  double calcXi(double at, double zp, double ap);

  /// Calculate C_p (Table 1 in ST'73 I)
  double cp(double at, double zt, double e, double ezero);

  /// Calculate f_1 (Eq 4 in ST'73 I)
  double f1(double atgt, double e, double ezero);
  /// Calculate the updated f_1 term (Eq 3-4 in ST'85)
  double f11(double e, double ezero);

  /// Calculate f_2 (Eq 5 in ST'73 I)
  double f2(double e);
  /// Calculate the updated f_2 term (Eq 9 in ST'85)
  double f22(double e, double ezero);

  /// Calculate f'_2 (Eq 6 in ST'73 I)
  double f2p(double e);

  /// Calculate f_3 (Eq 7 in ST'73 I)
  double f3(double zt, double e);
  /// Calculate the updated f_3 term (Eq 10 in ST'85)
  double f33(double e, double ezero);

  /// Calculate the updated f_4 term (Eq 11 in ST'85)
  double f44(double e, double ezero);

  /// Calculate Delta A_c (Eq 2 in ST'73 I)
  double dac(double at, double e, double ezero);

  /// Calculate the denominator term involving exp(-P*A_t) - (Table 1D in ST'73 I)
  double invpat(double p, double at);

  /// Calculate phi (Eq 
  double phi(double at, double e);

  /// Calculate f(A) (Eq 4a in ST'73 II)
  double fofa4(double zt, double at, double z,
	       double a, double atbar);

  /// Calculate the gamma exponent (Eq 7 in ST'73 II)
  double gamma(double z, double a, double zt, 
	       double at, double atbar, double e);

  /// Calculate Z given A (see p320 in ST'73 I).
  double zofa(double a);

  /// Calculate x^n safely
  double power(double x, double n);
  /// Calculate exp(x) safely
  double expfun(double x);

 protected:

 private:

};

#endif
