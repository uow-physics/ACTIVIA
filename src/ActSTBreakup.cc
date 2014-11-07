// Class to select cross-section formula

#include "Activia/ActSTBreakup.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActSTSpallation.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTBreakup::ActSTBreakup(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTBreakup::~ActSTBreakup() 
{
  // Destructor
  delete _updates; _updates = 0;
}

double ActSTBreakup::calcCrossSection(ActNucleiData* data) {
  
  if (data == 0) {
    cout<<"Error in ActSTBreakup. Data object is null"<<endl;
    return 0.0;
  }

  _sigma = 0.0;

  double at = data->getat();
  double zt = data->getzt();
  double a = data->geta();
  double z = data->getz();
  double atbar = data->getatbar();
  double e = data->gete();
  double ezero = data->getezero();
  int iz = data->getiz();

  // Region "B", eqns 12 et seq, p 351, in ST'73 II
  // Need to call spallation to first obtain sigez.
  data->setEnergy(ezero); // Set the energy to E0
  ActSTSpallation spallation("spal", false, _debug);
  double sigez = spallation.calcCrossSection(data);
  data->setEnergy(e); // reset the original energy

  double afac(0.0), bfac(0.0), fofa(0.0);
  if (iz < 11) {
    // Light evaporation products

    afac = 0.05; bfac = 0.9;

    if (iz <= 8 && iz > 5) {
      afac = 0.03; bfac = 0.7;
    } else if (iz == 5) {
      afac = 0.02; bfac = 0.6; // ST'85 p 878 update
    }

    double nzstar = _formulae.nzstar(z, a, zt, at, atbar);
    double expTerm = afac*(at - 56.0)*(nzstar - bfac);
    fofa = _formulae.expfun(expTerm);

    if (_debug == 1) {
      cout<<"nzstar, fofa = "<<nzstar<<" "<<fofa<<endl;
    }

    if (fofa < 1.0) {fofa = 1.0;}

  } else {
      
    // Equation 4a, p347, heavier products
    fofa = _formulae.fofa4(zt, at, z, a, atbar);
    
  }

  double fbeVal = _formulae.fbe(z, a, zt, at, atbar, e, ezero);

  if (_debug == 1) {
    cout<<"sigez,fofa,fbe="<<sigez<<" "<<fofa<<" "<<fbeVal<<endl;
  }

  _sigma = sigez*fofa*fbeVal;

  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  return _sigma;

}
