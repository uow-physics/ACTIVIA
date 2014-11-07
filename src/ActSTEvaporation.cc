// Class to calculate light evaporation products, eqns 8-11, paper II

#include "Activia/ActSTEvaporation.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTEvaporation::ActSTEvaporation(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTEvaporation::~ActSTEvaporation() 
{
  // Destructor
  delete _updates; _updates = 0;
}

double ActSTEvaporation::calcCrossSection(ActNucleiData* data) {

  if (data == 0) {
    cout<<"Error in ActSTEvaporation. Data object is null"<<endl;
    return 0.0;
  }

  _sigma = 0.0;

  double at = data->getat();
  double zt = data->getzt();
  double a = data->geta();
  double z = data->getz();
  int iz = data->getiz();
  int ia = data->getia();

  double atbar = data->getatbar();
  double e = data->gete();
  double ezero = data->getezero();

  double term1 = (0.003*a - 0.51)*a + z;
  double omega = _formulae.calcOmega(z, a, _name);
  double xi = _formulae.calcXi(at, z, a);
  double sigse = 13.0*_formulae.expfun(-1.8*term1*term1)*omega*xi;

  double nzstar = _formulae.nzstar(z, a, zt, at, atbar);
  double exp1 = 0.01*(at - 56.0)*(nzstar - 0.45);
  double fepsa = _formulae.expfun(exp1);
  if (fepsa < 1.0) {fepsa = 1.0;}

  double alpha(0.0);
  if (iz == 2 && ia == 6) {
    alpha = 0.8;
  } else if (iz == 3 && ia == 6) {
    alpha = 1.2;
  } else if (iz == 4 && ia == 7) {
    alpha = 1.8;
  }

  double eratio(0.0);
  if (fabs(ezero) > 1e-30) {eratio = e/ezero;}
  double fepse = fmin(_formulae.power(eratio, alpha), 1.0);

  if (_debug == 1) {
    cout<<"sigse,fepsa,fepse="<< sigse<<" "<<fepsa<<" "<<fepse<<endl;
  }

  _sigma = sigse*fepsa*fepse;

  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  return _sigma;

}
