// Class to hold data values for target and product nuclei and related
// quantities that will be useful for cross-section calculations.

#include "Activia/ActNucleiData.hh"
#include "Activia/ActMassDef.hh"
#include "Activia/ActTargetNuclide.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActNucleiData::ActNucleiData(ActNuclide* beamNuclide, ActTargetNuclide* targetIsotope, 
			     ActNuclide* product, double energy) : ActAbsData()
{
  // Constructor that calculates everything in one go
  this->setBeamData(beamNuclide);
  this->setTargetData(targetIsotope);
  this->setProductData(product);
  this->setEnergy(energy);
  this->setOtherQuantities();

}

ActNucleiData::ActNucleiData() 
{
  // Empty contructor. Use this if you want to add the target and product
  // data at different stages in a program 
  // (e.g. when using loops over targets and products)
  this->initialise();

} 

ActNucleiData::~ActNucleiData() 
{
  // Destructor
}

void ActNucleiData::initialise() {

  _beamNuclide = 0; _targetIsotope = 0; _product = 0;
  
  _zbeam = 0.0; _abeam = 0.0;
  _izbeam = 0; _iabeam = 0;

  _zt = 0.0; _at = 0.0; _atbar = 0.0; _z = 0.0; 
  _a = 0.0; _e = 0.0; _ezero = 0.0;
  _izt = 0; _iat = 0; _iz = 0; _ia = 0;
  _nt = 0.0; _int = 0; _n = 0.0; _in = 0;
  _dnpt = 0.0; _idnpt = 0; _dnp = 0.0; _idnp = 0;
  _x = 0.0; _y = 0.0; _ix = 0; _iy = 0; _ichg = 0;
  _mdtgt = 0.0; _thrse = 0.0;
  _fraction = 0.0;

}

void ActNucleiData::setTargetData(ActTargetNuclide* targetIsotope) {

  _targetIsotope = targetIsotope;

  double targetZ(0.0), targetA(0.0), targetMedianA(0.0);
  double fraction(0.0);

  if (targetIsotope != 0) {

    targetZ = targetIsotope->getfZ();
    targetA = targetIsotope->getA();
    targetMedianA = targetIsotope->getMedianA();
    fraction = targetIsotope->getFraction();

  }

  _zt = targetZ; _at = targetA; _atbar = targetMedianA;
  _fraction = fraction;
  _izt = (int) _zt; _iat = (int) _at;

  _nt = _at - _zt; _int = _iat - _izt;
  // N_n - N_p
  _dnpt = _nt - _zt; _idnpt = _int - _izt;

  ActMassDef massDef(targetZ, targetA);
  _mdtgt = massDef.calcDelta();

}

void ActNucleiData::setProductData(ActNuclide* product) {

  _product = product;

  if (product != 0) {

    _z = product->getfZ(); 
    _a = product->getA();    
    _iz = (int) _z; _ia = (int) _a;
  
    _n = _a - _z; _in = _ia - _iz;
    // N_n - N_p
    _dnp = _n - _z; _idnp = _in - _iz;  
  }

}

void ActNucleiData::setBeamData(ActNuclide* beamNuclide) {

  _beamNuclide = beamNuclide;

  if (beamNuclide != 0) {

    _zbeam = beamNuclide->getfZ();
    _abeam = beamNuclide->getA();

    _izbeam = (int) _zbeam;
    _iabeam = (int) _abeam;

  }  

}

void ActNucleiData::setThresholdEnergy() {

  double zcall(_z), acall(_a);
  if (_z < 9.0 || _a < 9.0) {
    zcall = _zt - _z; acall = _at - _a;
  }

  ActMassDef sbMass(zcall, acall);
  double mdprod = sbMass.calcDelta();

  _thrse = sbMass.thr(_zt, _at, _mdtgt, mdprod);

}

void ActNucleiData::setEZero() {

  // Set the critical energy E0 above which no significant
  // change in cross-sections is expected.
  // This uses ST'73 I, Eq 3
  //_ezero = 69.0*pow(_at, 0.867);
  // Update from ST'90, pg 368
  _ezero = 20.3*pow(_at, 1.169);
  if (_ezero > 4000.0) {_ezero = 4000.0;}

}

void ActNucleiData::setOtherQuantities() {

  this->setThresholdEnergy();
  this->setEZero();

  this->calcX();
  this->calcY();
  _ichg = this->calcichg(_ix, _iy);

}

void ActNucleiData::calcX() {
  _x = _at - _zt - _a + _z; 
  _ix = (int) _x;
}

void ActNucleiData::calcY() {
  _y = _zt - _z + 1.001;
  _iy = (int) _y;
}

void ActNucleiData::setXandY(double x, double y) {
  // Used in ActSTPeripheral class to select what
  // ypxn reaction to calculate
  _x = x; _y = y; _ix = (int) x; _iy = (int) y;
  _ichg = this->calcichg(_ix, _iy);
}

int ActNucleiData::calcichg(int ix, int iy) {
  int ichg = std::min(9, ix) + 10*iy;
  return ichg;
}

void ActNucleiData::print() {

  cout << "zt, at, atbar = " << _zt << ", " 
       << _at << ", " << _atbar
       << ", z, a, e, ezero = "<< _z << ", " 
       << _a << ", " << _e << ", " << _ezero << endl;
}
