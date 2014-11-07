// Calculate fission cross sections: use equations 2a for sigmas,  
// 4a for fofa, 5 for ffofe, 38a,b for phi(at,e), plug all into eqn 6
// for sigma (in Ap J Supp 220,25,335(1973))

#include "Activia/ActSTFissSpallGamma.hh"
#include "Activia/ActSTSpallation.hh"
#include "Activia/ActSTFission.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTFissSpallGamma::ActSTFissSpallGamma(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTFissSpallGamma::~ActSTFissSpallGamma() 
{
  // Destructor
  delete _updates; _updates = 0;
}

double ActSTFissSpallGamma::calcCrossSection(ActNucleiData* data) {

  if (data == 0) {
    cout<<"Error in ActSTFissSpallGamma. Data object is null"<<endl;
    return 0.0;
  }

  _sigma = 0.0;

  ActSTSpallation spallation("spal", false, _debug);
  _sigma = spallation.calcCrossSection(data);
  
  ActSTFission fission("fiss", false, _debug);
  double sigmaf = fission.calcCrossSection(data);

  double z = data->getz();
  double a = data->geta();
  double zt = data->getzt();
  double atgt = data->getat();
  double atbar = data->getatbar();
  double e = data->gete();

  double gamma = _formulae.gamma(z, a, zt, atgt, atbar, e);

  if (gamma > 0.0 && gamma < 1.0) {
    _sigma = _formulae.power(_sigma, gamma)*_formulae.power(sigmaf, (1.0 - gamma));
  } else if (gamma <= 0.0) {
    _sigma = sigmaf;
  }

  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  return _sigma;

}

