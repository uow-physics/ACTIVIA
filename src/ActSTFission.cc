// Calculate fission cross sections: use equations 2a for sigmas,  
// 4a for fofa, 5 for ffofe, 38a,b for phi(at,e), plug all into eqn 6
// for sigma (in Ap J Supp 220,25,335(1973))

#include "Activia/ActSTFission.hh"
#include "Activia/ActSTSpallation.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTFission::ActSTFission(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTFission::~ActSTFission() 
{
  // Destructor
  delete _updates; _updates = 0;
}

double ActSTFission::calcCrossSection(ActNucleiData* data) {

  if (data == 0) {
    cout<<"Error in ActSTFission. Data object is null"<<endl;
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

  int iat = data->getiat();
  int izt = data->getizt();

  // Calculate energy dependence factor, eqn 5 etc.
  double nvzstr = _formulae.nzstar(z, a, zt, at, atbar);
  double novrz(0.0), novrzt(0.0);
  if (std::fabs(z) > 1e-30) {novrz = (a - z)/z;}
  if (std::fabs(zt) > 1e-30) {novrzt = (at - zt)/zt;}

  // See last paragraph p 349
  if (novrz > novrzt) {
    nvzstr = _formulae.nzstar(zt, at, zt, at, atbar);
  }

  // See equations p 349
  double inve(0.0);
  if (std::fabs(e) > 1e-30) {inve = 1.0/e;}

  double ay = 19.0*_formulae.power((450.0*inve), 0.6);
  double pow1 = _formulae.power((350.0*inve), 4.0);
  double pow2 = _formulae.power((130.0*inve), 4.0);
  double dee = _formulae.expfun(pow1*(1.3 - nvzstr)/(1.0 + pow2));
  if (dee < 1.0) {dee = 1.0;}

  double gee = 2.25;
  if (e >= 200.0) {gee = 450.0*inve;}
  double aitch = 0.0;

  if (iat >= 180 && e <= 600.0) {
    
    double term = 0.68*(207.0 - at) + a - 105.0;
    aitch = fmax(0.0, 6.0e-4*term*(600.0 - e));
    if (e <= 450.0) {aitch = fmax(0.0, 0.09*term);}

  }

  if (aitch < 0.0) {aitch = 0.0;}

  // Done with preliminaries on ff of f, plug into eqn 5
  double logTerm = log(5.5*_formulae.power(inve, 0.07));
  double termAt = 0.0065*(207.0 - at); // update from ST'85, Eq 13.
  double absPow = std::fabs(e - 700.0)/700.0;
  double term2(0.0);
  if (std::fabs(at) > 1e-30) {term2 = (a - 0.46*at)/(0.15*at);}
  double expPow = ay*(nvzstr - logTerm - termAt*_formulae.expfun(-absPow));
  expPow -= gee*term2*term2;
  expPow -= aitch;
  double ffofe = dee*_formulae.expfun(expPow);

  if (_debug == 1) {
    cout<<"logTerm = "<<logTerm<<", termAt = "<<termAt<<", absPow = "<<absPow<<endl;
    cout<<"nvzstr = "<<nvzstr<<endl;
    cout<<"expPow = "<<expPow<<", ay = "<<ay<<", dee = "<<dee<<endl;    
    cout<<"ffofe,dee,gee,aitch = "<<ffofe<<" "<<dee<<" "<<gee<<" "<<aitch<<endl;
  }

  // Apply limiting factors ffofe max from last paragraph p 349
  if (e <= 450.0) {
    ffofe = fmin(4.0, ffofe);
  } else if (e > 450.0 && e < 1800.0) {
    ffofe = fmin(1800.0*inve, ffofe);
  } else if (e >= 1800.0) {
    ffofe = fmin(1.0, ffofe);
  }
  
  // Update from ST'90, p369
  if (izt >= 76 && izt <= 80) {
    double eratio(0.0);
    if (std::fabs(ezero) > 1e-30) {eratio = e/ezero;}
    double dummyfac = 1050.0*inve + eratio;
    dummyfac = fmin(6.0, dummyfac);
    ffofe *= dummyfac;
  }

  // Spallation cross-section
  data->setEnergy(ezero); // set the energy to E0
  ActSTSpallation spallation("fiss", false, _debug); // need name fiss to have m = 0 in spallation sigma
  double sigmas = spallation.calcCrossSection(data);
  data->setEnergy(e); // reset the original energy

  // Put it all together
  double fofa4 = _formulae.fofa4(zt, at, z, a, atbar);
  double phi = _formulae.phi(at, e);

  _sigma = sigmas*fofa4*ffofe*phi;

  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  return _sigma;

}

