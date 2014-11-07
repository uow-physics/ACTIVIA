#ifndef ACT_ST_SELECT_HH
#define ACT_ST_SELECT_HH

#include "Activia/ActNucleiData.hh"

#include <vector>
#include <string>

/// \brief Select which Silberberg-Tsao formulae to use
///
/// This class decides which region is appropriate for the given target-product
/// nuclei, using the domains of applicability shown in Fig 4, p339, and Table 1 in ST'73 II.

class ActSTSelect {

 public:

  /// Construct the selection method for the given set of target-product nuclei data.
  ActSTSelect(ActNucleiData& data);
  virtual ~ActSTSelect();

  /// Find which Silberberg-Tsao formula should be used
  std::string findFormula();

  /// Calculate Delta A_c, Eq 2, ST'73 I.
  double delac(double at, double e, double ezero);

  /// Calculate (N/Z)_c, Eq 3, ST'73 II.
  double nzc(double at, double zt, double dela, double ezero, double atbar);

 protected:

 private:

  ActSTSelect() {;}

  double _zt, _at, _atbar, _z, _a, _e, _ezero;
  int _iat, _ia, _iz, _izt, _ix, _iy;

  ActNucleiData _data;

};

#endif
