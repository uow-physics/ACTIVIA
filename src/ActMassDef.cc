// This class estimates the target and product masses 
// for energy threshold calculations. It uses the semi-empirical mass formula
// from Seeger, Nuc. Phys 25, 1, (1961) as outlined in Enge,
// Intro. to Nuclear Physics. It returns the mass excess
// in MeV/c2 for the given z,a nuclide.

#include "Activia/ActMassDef.hh"
#include "Activia/ActConstants.hh"
#include "Activia/ActClassify.hh"

#include <cmath>
#include <algorithm>

ActMassDef::ActMassDef(double z, double a)
{
  // Constructor
  _z = z; _a = a;

  this->setUp();

}

ActMassDef::~ActMassDef() 
{
  // Destructor
}

void ActMassDef::setUp() {

  // Set-up constants
  _cmass = 12.*1.6605402*(2.99792458*2.99792458)*(10.e-4)/1.602189e-6;
  _omass = -4.737 + (4.0*_cmass/3.0);

  _alpha = 17.06, _beta = 33.61, _gamma = 25.00, _eta = 59.54; _chi = 13.51;

  _ksi.clear(); _nu.clear(); _phi.clear(); _magicn.clear();

  _ksi.push_back(4.008); _ksi.push_back(-0.508); _ksi.push_back(-7.636);
  _ksi.push_back(-15.63); _ksi.push_back(-27.59);

  _nu.push_back(-0.428); _nu.push_back(2.331); _nu.push_back(0.496);
  _nu.push_back(-2.284); _nu.push_back(2.66);

  _phi.push_back(-1.389); _phi.push_back(-0.463); _phi.push_back(1.950);
  _phi.push_back(10.67); _phi.push_back(26.99);

  _magicn.push_back(8.0); _magicn.push_back(20.0); _magicn.push_back(50.0);
  _magicn.push_back(82.0); _magicn.push_back(126.0); _magicn.push_back(184.0);

  _nmagicn = (int) _magicn.size();
  

}

double ActMassDef::calcDelta() {

  // Convert mass from Seeger's 16-O mass scale to 12-C mass scale
  double xmass = this->calcMass();
  double delta = xmass + (_a*_omass/16.0) - (_a*_cmass/12.0);
  return delta;

}

double ActMassDef::calcMass() {

  // a and z should be non-zero, so there should be no problems
  // with dividing by zero etc...

  if (_a < 1e-6 || _z < 1e-6) {return 0.0;}

  double n = _a - _z;
  
  // First set xmass to Seeger formula without shell corrections
  double xmass = 8.3674*n + 7.5848*_z - _alpha*_a;

  double oneThird = 1.0/3.0;
  double twoThirds = 2.0*oneThird;
  double zExp23 = pow(_z, twoThirds);
  double aExp23 = pow(_a, twoThirds);
  double aExp13 = pow(_a, oneThird);

  double term1 = 0.8076*_z*_z*(1.0 - (0.7636/zExp23) - (2.29/aExp23))/aExp13;
  double term2 = _gamma*aExp23;
  double a2z = _a - 2.0*_z;
  double term3 = (_beta - (_eta/aExp13))*(a2z*a2z + 2.0*fabs(a2z))/_a;

  xmass += term1 + term2 + term3;

  // Now calculate j and k - these are the indices of the last filled
  // neutron and proton shell, respectively

  int k(-1), j(-1), imagic(0);

  for (imagic = 0; imagic < _nmagicn; imagic++) {
    if (_z < _magicn[imagic]) {break;}
    k++;
  }

  for (imagic = 0; imagic < _nmagicn; imagic++) {
    if (n < _magicn[imagic]) {break;}
    j++;
  }

  // Now calculate normalised n and z: nprime, zprime
  double nprime(1.0), zprime(1.0);

  if (k >= 0 && k < _nmagicn-1) {
    double denom = _magicn[k+1] - _magicn[k];
    if (fabs(denom) > 1e-30) {
      zprime = (_z - _magicn[k])/denom;
    }
  } 
  if (j >= 0 && j < _nmagicn-1) {
    double denom = _magicn[j+1] - _magicn[j];
    if (fabs(denom) > 1e-30) {
      nprime = (n - _magicn[j])/denom;
    }
  } 

  double zarg = zprime*ActConstants::pi;
  double narg = nprime*ActConstants::pi;
  double sinZ = sin(zarg);
  double sinN = sin(narg);
  double sin2Z = sin(2.0*zarg);
  double sin2N = sin(2.0*narg);

  // Now calculate s(jk)
  double sjk = _ksi[j]*sinN + _ksi[k]*sinZ + _nu[j]*sin2N + _nu[k]*sin2Z;
  sjk += (_phi[j] + _phi[k])*sinN*sinZ + _chi;

  // Add shell correction to xmass
  xmass -= sjk;

  int ia = (int) _a;
  int iz = (int) _z;
  ActClassify classify(ia, iz);
  int ioe = classify.classif();

  if (ioe == ActClassify::EvenEven) {
    xmass -= del(_a);
  } else if (ioe == ActClassify::OddOdd) {
    xmass += del(_a);
  }

  return xmass;

}

double ActMassDef::del(double a) {

  double term = pow((280.0-a), 2.3);
  double del = 5.00e-6*term + 0.687;
  return del;

}

double ActMassDef::thr(double zt, double at, double mdtgt, double mdprod) {

  // Calculate threshold energy for zt,at + p -> z,a + p + whatever, where
  // whatever = max permitted alphas, at most one 3-H and at most one 3-He,
  // plus required additional p,n.      
  // Also, assume incident particle remains after reaction, so things
  // like (n,alpha) reactions are not handled. An (n,alpha) reaction
  // would be calculated as (n,n+3-He).

  double thrse = 0.0;

  int nalpha(0), n3h(0), n3he(0);

  double defalp(2.42492), def3he(14.9309), def3h(14.9323);
  double defn(8.071246), defp(7.288868);

  // Calculate changes of z and a in this reaction

  int dela = (int) (at - _a);
  int delz = (int) (zt - _z);
  int deln = dela - delz;

  if (delz >= 2 && deln >= 2) {nalpha = std::min(delz/2,deln/2);}

  // Are there enough knocked out nucleons left after forming alphas to
  // make a 3-He too?

  int delns = deln - 2*nalpha;
  int delzs = delz - 2*nalpha;
  if (delzs >= 2 && delns >= 1) {
    n3he = 1;
    delzs -= 2;
    delns--;
  }

  // Are there still enough knocked out nucleons left to make a 3-H?

  if (delzs >= 1 && delns >= 2) {
    n3h++;
    delzs--;
    delns -= 2;
  }

  // Make up balance with free n and p
  int nn = delns;
  int np = delzs;

  thrse = mdprod + nalpha*defalp + def3he*n3he + def3h*n3h + 
          nn*defn + np*defp - mdtgt - defp;

  return thrse;

}
