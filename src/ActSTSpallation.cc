// Class to calculate cross-sections (mb) for various targets Z > 28

#include "Activia/ActSTSpallation.hh"
#include "Activia/ActClassify.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTSpallation::ActSTSpallation(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  this->setUp();
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTSpallation::~ActSTSpallation() 
{
  // Destructor
  delete _updates; _updates = 0;
}

void ActSTSpallation::setUp() {

  _eta.clear(); _eta.resize(4);
  int i, j;
  for (i = 0; i < 4; i++) {
    _eta[i].resize(4);
    for (j = 0; j < 4; j++) {
      _eta[i][j] = 1.0;
    }
  }

  for (i = 0; i < 4; i++) { _eta[i][0] = 1.0;}

  _eta[0][1] = 1.15; _eta[1][1] = 1.15; 
  _eta[2][1] = 0.9; _eta[3][1] = 0.8;
  _eta[0][2] = 1.25; _eta[1][2] = 0.9;
  _eta[2][2] = 1.0; _eta[3][2] = 0.85;
  _eta[0][3] = 1.25; _eta[1][3] = 0.9;
  _eta[2][3] = 1.0; _eta[3][3] = 0.85;

  _EMaxLimit = 3000.0; // asymptotic high-energy threshold (MeV)
  _E1GeV = 1000.0;
  _EMaxDiff = _EMaxLimit - _E1GeV;

  _etasel = 0.0;
}

double ActSTSpallation::calcCrossSection(ActNucleiData* data) {

  if (data == 0) {
    cout<<"Error in ActSTSpallation. Data object is null"<<endl;
    return 0.0;
  }

  _sigma = 0.0;

  double at = data->getat();
  double zt = data->getzt();
  double a = data->geta();
  double z = data->getz();
  double atbar = data->getatbar();
  double ezero = data->getezero();

  int izt = data->getizt();
  int iz = data->getiz();
  int ia = data->getia();
  int iat = data->getiat();

  double energy = data->gete();

  // First get classification for pairing factor eta
  ActClassify classify(ia, iz);
  int ioe = classify.classif();
  if (_debug == 1) {
    cout<<"a,z,ia,iz,ioe = "<<a<<" "<<z<<" "<<ia<<" "<<iz<<" "<<ioe<<endl;
  }
  _etasel = _eta[ioe][3];
  // Update: for A = 100-180, eta = 1
  if (ia >= 100 && ia <= 180) {_etasel = 1.0;}

  double dela(0.0);
  _sigma = this->calcSpallSigma(data, dela);

  // Need to calculate sigma(Ezero)
  double dela0(0.0);
  data->setEnergy(ezero);
  double sige0 = this->calcSpallSigma(data, dela0);
  if (_debug == 1) {cout<<"sige0 = "<<sige0<<endl;}
  data->setEnergy(energy);

  // ST'73 II, pg 339
  double delac = _formulae.dac(at, 300.0, ezero);
  double delaCut = delac*(200.0 + energy)*0.002;
  if (_debug == 1) {cout<<"delaCut = "<<delaCut<<endl;}

  if (iat >= 69 && energy < 300.0 && dela > delaCut) {

    // Implement top of p 339, eqn 2b
    double quo1(0.0), quo2(0.0);
    if (fabs(sige0) > 1e-30) {quo1 = _sigma/sige0;}
    double biggVal = _formulae.bigg(z, a, zt, at, atbar, energy);
    double fbeVal = _formulae.fbe(z, a, zt, at, atbar, energy, ezero);
    
    if (fabs(biggVal) > 1e-30) {quo2 = fbeVal/biggVal;}
    
    if (quo1 < quo2) {
      if (_debug == 1) {cout<<"quo1 = "<<quo1<<", quo2 = "<<quo2<<endl;}
      _sigma = sige0*quo2;
    }

    if (_debug == 1) {
      cout<<"values = "<<biggVal<<" "<<fbeVal<<" "<<quo1<<" "<<quo2<<" "<<sige0<<endl;
      cout<<"sigmaval = "<<_sigma<<endl;
    }

  }

  // The following is needed for interpolation scheme, last
  // paragraph pg 338. No need to re-apply eqn 2b
  // for sigmas at 1 GeV and EMaxLimit (3 GeV), since E > 300 MeV.

  if (dela >= 10.0) {

    if (izt > 28 && energy > _E1GeV) {

      // Get the sigma at the maximum energy (3GeV). 
      // This is used in both if conditions below.
      double sigEMax = fmin(sige0, _sigma);

      if (energy >= _EMaxLimit) {

        _sigma = sigEMax;
        if (_debug == 1) {cout<<"sigb = "<<_sigma<<endl;}

      } else {

	// Linear interpolation between 1 GeV and EMaxLimit (3 GeV), otherwise the
	// calculated cross-section is too high in this range (see ST'73 II, p 338).
	// Sigma at the EMaxLimit if set to the minimum of sige0 or _sigma.

	// Get sigma at 1 GeV
	double dela1(0.0);
	data->setEnergy(_E1GeV);
	double sig1 = this->calcSpallSigma(data, dela1);

	// We don't need sig1 = fmin(sige0, sig1) here since we know that
	// the sigma at 1 GeV is OK when calculated by the spallation formulae.
	// If we have the fmin condition, we get a discontinuity at 1 GeV.

	if (_debug == 1) {cout<<"sig1 = "<<sig1<<endl;}

	// Do the linear interpolation
        double grad = 0.0;
        if (fabs(_EMaxDiff) > 1e-10) {grad = (sigEMax - sig1)/_EMaxDiff;}
        _sigma = grad*(energy - _E1GeV) + sig1;
 
        if (_debug == 1) {cout<<"sigc = "<<_sigma<<endl;}

	// Reset the energy for the nuclei data pointer
	data->setEnergy(energy);

      }
    } 
  }

  if (_debug == 1) {cout<<"spall sigma = "<<_sigma<<endl;}
  
  // Update: reduction in sigma for A=180-200
  if (ia >= 180 && ia <= 200) {
    double F = 1.0 - 2e-3*(at - 100.0);
    _sigma *= F;
  }

  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  return _sigma;
  
}

double ActSTSpallation::calcSpallSigma(ActNucleiData* data, double& dela) {

  if (data == 0) {
    dela = 0.0;
    return 0.0;
  }

  // Check for large deltaA correction, Eq 2, p319
  double at = data->getat();
  double a  = data->geta();
  double zt = data->getzt();
  double atbar = data->getatbar();
  double z = data->getz();
  int iz = data->getiz();
  int ia = data->getia();
  int izt = data->getizt();

  double energy = data->gete();
  double ezero = data->getezero();

  dela = at - a;
  double delac = _formulae.dac(at, energy, ezero);
  if (dela > delac) {dela = delac;}
  
  // Now define parameters for the cross-section calculation
  // Use Table 1D, ST'73 I and updates
  double sratio(0.0);
  if (fabs(zt) > 1e-30) {sratio = (at - atbar)/zt;}
  
  // Using the update from ST'90, pg 368
  double s = 0.482 - 0.07*sratio;
  double t = 0.00028;
  double zstar = ((3.0e-7*a + t)*a - s)*a + z;
  double nu(0.0);
  if (zstar < -1.0) {
    nu = 1.3;
  } else if (zstar >= -1.0 && zstar <= 1.0) {
    nu = 1.5;
  } else {
    nu = 1.75;
  }

  double p = this->calcPValue(data);
  double f1Val = _formulae.f1(at, energy, ezero);
  double f2Val = _formulae.f2(energy);
  double atpow = _formulae.power(at, 0.367);

  if (energy >= fmin(_EMaxLimit, ezero)) {f1Val = 1.0;}

  //Highly uncertain p formula from ST'90, p 368
  //if (energy > _E1GeV && energy < _EMaxLimit) {
  //double pTerm1 = 1000.0/energy;
  //double pTerm2 = 1.886*log(_at) - 2.732;
  //p = 0.098*_formulae.power(pTerm1, pTerm2);
  //}

  // Implement p 320 pgh 2- spallation production of 7-Be is not suppressed by exp-p*dela
  double psup(p);
  if (iz >= 2 && iz <= 4 && ia >= 6 && ia <= 12 && energy >= 1250.0) {psup = 0.0;}
  
  double r = 1.29*_formulae.power(a, 0.15);
  if (ia >= 40) {r = 11.8*_formulae.power(a, -0.45);}

  double sig0 = 144.0*p*f1Val*f2Val*atpow;

  double invpatVal = _formulae.invpat(p, at);
  sig0 *= invpatVal;

  if (_debug == 1) {cout<<"sig0 = "<<sig0<<endl;}

  // Calculate M of last-but-one paragraph p 338 for magic number yield curve shift
  double m = this->calcMValue(_name, izt, iz);

  double aprm = a + m;
  
  double dacVal = _formulae.dac(at, ezero, ezero);

  double absTerm = fabs(((3.0e-7*aprm + t)*aprm - s)*aprm + z);
  double omega = _formulae.calcOmega(z, a, _name);
  double rpow = r*_formulae.power(absTerm, nu);
  double expTerm = -psup*dela - rpow;
  
  double omegaEta = omega*_etasel;
  double prod = _formulae.expfun(expTerm)*omegaEta;

  // Correction factors
  // Update from ST'85, p876 Eq 5-11
  double corr(1.0);
  if (izt > 28 && ia <= 56 && dela > dacVal) {
    corr = _formulae.f22(energy, ezero);
  }
  if (izt > 21 && dela >= 7.0 && dela <= (dacVal - 13.0)) {
    corr = _formulae.f33(energy, ezero);
  }
  if (izt > 90 && ia > 56 && dela >= 7.0) {
    corr = _formulae.f44(energy, ezero);
  }
  // Production of Boron from targets with zt >= 21
  if (izt >= 21 && iz == 5) {
    double corr1 = _formulae.f11(energy, ezero);
    double corr2 = _formulae.f22(energy, ezero);
    double corrProd = corr1*corr2;
    if (corrProd > 0.0) {corr = sqrt(corrProd);}
  }
  //Update from ST'90, p368, Eq 17. Only do for 76<=Zt<=83.
  if (izt >= 76 && izt <= 83 && energy > 200.0 && energy < 1001.0) {
    double fETerm = _formulae.power(energy*1.0e-3, -(2.0/3.0));
    double deltaA(dela);
    if (deltaA < 15.0) {deltaA = 15.0;}
    double fdAE = 0.0;
    if (at > 0.0) {fdAE = (deltaA/(0.14*at))*fETerm;}
    if (fdAE > 2.0) {fdAE = 2.0;}
    corr *= fdAE;

  }
  
  // Ad-hoc corrections to match data points...
  if (izt == 52 && iz == 27) {
    // Co from Te targets
    corr *= 0.5;
  } else if (izt == 32) {
    // Ge targets
    if (iz == 27) {
      corr *= 1.75;
    } else if (iz == 30) {
      corr *= 0.5;
    }
  } else if (izt == 29) {
    // Cu targets
    if (iz == 27 && ia == 56) {
      corr *= 1.25;
    } else if (iz == 21 && ia == 46) {
      corr *= 2.0;
    }
  }

  double sigma = sig0*prod*corr;

  return sigma;

}

double ActSTSpallation::calcPValue(ActNucleiData* data) {

  if (data == 0) {return 0.0;}

  double p(0.0);

  double zt = data->getzt();
  double at = data->getat();
  double energy = data->gete();
  double ezero = data->getezero();

  double atpow09 = _formulae.power(at, -0.9);

  if (energy >= fmin(_EMaxLimit, ezero)) {
    p = 1.97*atpow09;
  }
  
  if (energy < ezero) {
    double cpVal = _formulae.cp(at, zt, energy, ezero);
    p = 20.0*cpVal*_formulae.power(energy, -0.77);
  }

  return p;

}

double ActSTSpallation::calcMValue(std::string name, int izt, int iz) {

  double m(0.0);
  if (name != "fiss") {
    
    if (izt > 76) {
      if (iz > 64) {
	m = 1.0;
      } else if (iz == 63) {
	m = 1.9;
      } else if (iz == 64) {
	m = 1.7;
      }
    }
    if (iz == 40) {
      m = -0.7; // updated from -1.1 using ST'90, p368
    } else if (iz == 42) {
      m = 0.8;
    } else if (iz == 53) {
      m = -0.4;
    } else if (iz == 55) {
      m = 1.7;
    } else if (iz == 60) {
      m = -1.1;
    } else if (iz == 61) {
      m = -1.3;
    } else if (iz == 63) {
      m = 0.9;
    } else if (iz == 64) {
      m = 0.7;
    }
  }
  
  return m;

}
