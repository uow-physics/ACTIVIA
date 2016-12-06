// Class to calculate the cross-sections (mb) for Z<29 targets

#include "Activia/ActSTSLite.hh"
#include "Activia/ActClassify.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTSLite::ActSTSLite(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTSLite::~ActSTSLite() 
{
  // Destructor
  delete _updates; _updates = 0;
}

void ActSTSLite::setUp(ActNucleiData& data) {

  _zt = data.getzt();
  _at = data.getat();
  _atbar = data.getatbar();
  _z = data.getz();
  _a = data.geta();
  _e = data.gete();
  _ezero = data.getezero();

  _izt = data.getizt();
  _iz = data.getiz();
  _ia = data.getia();
  _iat = data.getiat();
  
  _logat = 0.0; if (_at > 0.0) {_logat = log(_at);}
  _loge = 0.0; if (_e > 0.0) {_loge = log(_e);}

  this->initLiteParam();

  _eta.clear(); _eta.resize(4);
  int i, j;
  for (i = 0; i < 4; i++) {
    _eta[i].resize(4);
    for (j = 0; j < 4; j++) {
      _eta[i][j] = 1.0;
    }
  }

  // _eta = 1 for A = 100-180
  if (_ia < 100 || _ia > 180) {
   
    for (i = 0; i < 4; i++) { _eta[i][0] = 1.0;}
    
    _eta[0][1] = 1.15; _eta[1][1] = 1.15; 
    _eta[2][1] = 0.9; _eta[3][1] = 0.8;
    _eta[0][2] = 1.25; _eta[1][2] = 0.9;
    _eta[2][2] = 1.0; _eta[3][2] = 0.85;
    _eta[0][3] = 1.25; _eta[1][3] = 0.9;
    _eta[2][3] = 1.0; _eta[3][3] = 0.85;
  }
}

void ActSTSLite::initLiteParam() {

  // Initialise common parameters in the "lite" algorithms
  _t = 0.0; _s = 0.0; _p = 0.0; _r = 0.0;
  _nu = 0.0; _etasel = 0.0; _sig0 = 0.0; _corr = 1.0;
  
}

double ActSTSLite::calcCrossSection(ActNucleiData* data) {

  if (data == 0) {
    cout<<"Error in ActSTSLite. Data object is null"<<endl;
    return 0.0;
  }

  // We assign the values from the ActSTData object to
  // private data members, since they will get passed
  // around several internal functions. We do this,
  // and initialise other variables, in the setUp function.
  this->setUp(*data);

  // Initialise the cross-section value
  _sigma = 0.0;

  if (_name == "lit5") {

    // Special case for using geometric mean of cross-sections (for Ca and Sc targets)
    double sigma1 = this->calcSLiteSigma("lit3");
    double sigma2 = this->calcSLiteSigma("lit4");
    if (sigma1 > 0.0 && sigma2 > 0.0) {
      _sigma = sqrt(sigma1*sigma2);
    }
    
  } else {

    _sigma = this->calcSLiteSigma(_name);
    
  }    

  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  return _sigma;

}

double ActSTSLite::calcSLiteSigma(std::string name) {

  double sigma(0.0);

  // First get classification for pairing factor eta
  ActClassify classify(_ia, _iz);
  int ioe = classify.classif();

  // Next check for large deltaA correction; eqn 2, ST'73 I
  double dela = _at - _a;
  double at36 = _at - 36.0;
  double delac = 0.045*at36*(_logat + 1.23) + 31.5;
  
  if (_e < _ezero) {
    delac = 0.052*at36*(_loge - 3.17) + 31.5;
  }
  if (dela > delac) {dela = delac;}
  
  // Now get started with the cross-section calculations
  
  if (name == "lit1") {
    this->calcLite1Values(ioe);
  } else if (name == "lit2") {
    this->calcLite2Values(ioe);
  } else if (name == "lit3") {
    this->calcLite3Values(ioe);
  } else if (name == "lit4") {
    this->calcLite4Values(ioe);
  } else {
    // Name is not recognised. Return zero cross-section.
    return 0.0;
  }
  
  double absVar = std::fabs((_t*_a - _s)*_a + _z);
  
  double omega = _formulae.calcOmega(_z, _a, name);
  double xi = _formulae.calcXi(_at, _z, _a);
  
  sigma = _sig0*_formulae.expfun(-_p*dela - _r*_formulae.power(absVar, _nu))*omega*_etasel*xi*_corr;

  if (_debug == 1) {
    cout<<"sig0,p,dela,r,z,s,a,t,nu,omega,etasel,xi,corr"<<endl;
    cout<<_sig0<<" "<<_p<<" "<<dela<<" "<<_r<<" "<<_z<<" "<<_s<<" "<<_a
	<<" "<<_t<<" "<<_nu<<" "<<omega<<" "<<_etasel<<" "<<xi<<" "<<_corr<<endl;
    cout<<"SLite sigma = "<<sigma<<endl;
  }
  
  return sigma;

}

void ActSTSLite::calcLite1Values(int ioe) {

  this->initLiteParam();

  // Light target nuclei, table 1A, AP J 220, 317 (1973)
  _r = 1.8;
  // S parameter includes update ST'85, p875, Eq 1
  double atzt(0.0);
  if (std::fabs(_zt) > 1e-30) {atzt = _at/_zt;}
  double atzt2 = atzt - 2.0;
  if (atzt2 > 0.0) {
    _s = -0.32*_formulae.power(atzt2, 1.4) + 0.54;
  } else {
    _s = 0.32*_formulae.power(-atzt2, 1.4) + 0.54;
  }
  
  _t = 0.003;
  _nu = 2.0;
  _etasel = _eta[ioe][0];
  _sig0 = 0.0;
  _corr = 1.0;
  _p = 0.0;

  if (_e < 1250.0) {

    double e1250 = 1.0 - _e/1250.0;
    double f3Val = _formulae.f3(_zt, _e);
    _sig0 = 13.0*f3Val*_formulae.expfun(1.15*e1250);
    _p = 0.16*e1250;
    if (_izt <= 20) {_r = 10.7*_formulae.power(_e, -0.25);}

  } else {
    _sig0 = 13.0; _p = 0.0;
  }

  // Update from ST'85 Eqs 2-4
  if (_izt <= 28 && _izt >= 21) {

    if (_e < _ezero || _e > 2500.0) {
      _corr = _formulae.f11(_e, _ezero);
    } else {
      double term1 = _formulae.f11(_e, _ezero);
      double eratio(0.0);
      if (std::fabs(_ezero) > 1e-30) {eratio = 2262.0*_e/_ezero;}
      double term2 = _formulae.f11(eratio, _ezero);
      double product = term1*term2;
      if (product > 0.0) {_corr = sqrt(product);}
    }
  }
}

void ActSTSLite::calcLite2Values(int ioe) {

  this->initLiteParam();

  // Light target nuclei, table 1B, AP J 220, 318 (1973)
  double g(1.0), elit2(1250.0);
  if (_ezero > 1250.0) {elit2 = _ezero;}
  if (_e < elit2 && _e > 0.0) {
    _p = 2.6/sqrt(_e);
    _r = 10.2*_formulae.power(_e, -0.26);
  } else {
    _p = 0.075;
    if (_ezero > 1250.0) {_p = 0.77*_formulae.power(_at, -0.6667);}
    _r = 1.6;
  }
  
  // S parameter includes update ST'85, p875, Eq 1
  double atzt(0.0);
  if (std::fabs(_zt) > 1e-30) {atzt = _at/_zt;}
  double atzt2 = atzt - 2.0;
  if (atzt2 > 0.0) {
    _s = -0.26*_formulae.power(atzt2, 1.4) + 0.502;
  } else {
    _s = 0.26*_formulae.power(-atzt2, 1.4) + 0.502;
  }
  _t = 0.0005;
  _nu = 2.0;

  _etasel = _eta[ioe][1];
  double term1 = _formulae.power(_at, (2.0/3.0)) - 1.0;
  double term2 = -0.3*(_logat - log(20.0)) + 1.0;
  if (std::fabs(_r) > 1e-30) {_sig0 = 28.0*term1*term2;}

  // Update from ST'90, p369
  if (_izt >= 14) {
    if (_e > 500.0) {
      g = 0.12*(_zt - 13.0) + 1.0;
    } else if (_e <= 500.0 && _e > 200.0) {
      g = 0.4*(_zt - 13.0)*(_e - 200.0)*1e-3 + 1.0;
    } else {
      g = 1.0;
    }
  }

  double denom = -_formulae.expfun(-_p*_at) + 1.0;
  if (_r > 1e-30 && std::fabs(denom) >= 1e-30) {
    _sig0 *= g*_p*sqrt(_r)/denom;
  }

}

void ActSTSLite::calcLite3Values(int ioe) {

  this->initLiteParam();
  
  // Light target nuclei, table 1C, AP J 220, 318 (1973)
  double g(1.0);
  if (_e < 1250.0) {
    _r = 10.2*_formulae.power(_e, -0.26);
  } else {
    _r = 1.6;
  }

  _p = 0.77*_formulae.power(_at, (-2.0/3.0));
  // Update from ST'90, p369
  if (_e < _ezero) {_p = 1.97*_formulae.power(_at, -0.9);}

  double atzt(0.0);
  if (std::fabs(_zt) > 1e-30) {atzt = _at/_zt;}
  _s = -0.06*(atzt - 2.0) + 0.502; // update from ST'85, p874
  _t = 0.0005;
  _nu = 2.0;
  
  _etasel = _eta[ioe][2];

  double term1 = _formulae.power(_at, (2.0/3.0)) - 1.0;
  double term2 = -0.3*(_logat - log(20.0)) + 1.0;

  double f2pVal = _formulae.f2p(_e);
  _sig0 = 28.0*f2pVal*term1*term2;

  // Update from ST'90, p369
  if (_izt <= 19) {
    if (_e > 500.0) {
      g = 0.12*(_zt - 13.0) + 1.0;
    } else if (_e <= 500.0 && _e > 200.0) {
      g = 0.4*(_zt - 13.0)*(_e - 200.0)*1e-3 + 1.0;
    } else {
      g = 1.0;
    }
  }

  double denom = -_formulae.expfun(-_p*_at) + 1.0;
  if (_r > 1e-30 && std::fabs(denom) > 1e-30) {
    _sig0 *= (g*_p*sqrt(_r)/denom);
  }

}

void ActSTSLite::calcLite4Values(int ioe) {

  this->initLiteParam();

  double atzt(0.0);
  if (std::fabs(_zt) > 1e-30) {atzt = (_at - _atbar)/_zt;}
  // Updates from ST'90, p368
  _s = -0.07*atzt + 0.482;
  _t = 0.00028;

  double zstar = ((3.0e-7*_a + _t)*_a - _s)*_a + _z;
  if (zstar < -1.0) {
    _nu = 1.3;
  } else if (zstar >= -1.0 && zstar <= 1.0) {
    _nu = 1.5;
  } else {
    _nu = 1.75;
  }

  _etasel = _eta[ioe][3];

  if (_e >= fmin(3000.0, _ezero)) {_p = 1.97*_formulae.power(_at, -0.9);}

  if (_e < _ezero) {
    double cpVal = _formulae.cp(_at, _zt, _e, _ezero);
    _p = 20.0*cpVal*_formulae.power(_e, -0.77);
  }

  _r = 1.29*_formulae.power(_a, 0.15);
  if (_ia >= 40) {_r = 11.8*_formulae.power(_a, -0.45);}

  double dacVal = _formulae.dac(_at, _ezero, _ezero);
  double delad = _at - _a;

  if (delad > dacVal) {
    _corr = _formulae.f22(_e, _ezero);
    cout<<"lit4 corr f22 = "<<_corr<<endl;
  }

  if (delad >= 7.0 && delad <= (dacVal - 13.0)) {
    _corr = _formulae.f33(_e, _ezero);
    cout<<"lit4 corr f33 = "<<_corr<<endl;
  }

  double f1Val = _formulae.f1(_at, _e, _ezero);
  double f2Val = _formulae.f2(_e);
  _sig0 = 144.0*_p*f1Val*f2Val*_formulae.power(_at, 0.367);

  double invpatVal = _formulae.invpat(_p, _at);
  _sig0 *= invpatVal;

}
