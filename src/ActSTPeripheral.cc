// Class to calculate the peripheral reactions acc'd to Silberberg and Tsao, 
// AP J Supp 220(I) 335 (1973) + ST'77, ST'85, ST'90
// Cross section given by sig=sig(ezero)*H(E)*Y(At,Zt)     

#include "Activia/ActSTPeripheral.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActSTSLite.hh"
#include "Activia/ActSTSpallation.hh"
#include "Activia/ActSTSLite.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTPeripheral::ActSTPeripheral(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTPeripheral::~ActSTPeripheral() 
{
  // Destructor
  delete _updates; _updates = 0;
}

double ActSTPeripheral::calcCrossSection(ActNucleiData* data) {

  // Peripheral reactions of the form ypxn

  if (data == 0) {
    cout<<"Error in ActSTPeripheral. Data object is null"<<endl;
    return 0.0;
  }

  // Initialise the cross-section value
  _sigma = 0.0;

  // Calculate the number of neutrons lost and symbolic ichg
  double x0 = data->getx();
  double x(x0);

  // Add one to proton digit to make it more readable.
  // Prevent large x from causing carry-over to next z

  double y0 = data->gety();
  double y(y0);
  int ix = data->getix();
  int iy = data->getiy();

  // No (n, (x+2)p) reactions
  if (iy >= 3 && ix <= -1) {
    ix = 0; x = 0.0;
  }

  if (iy == 2 && ix == -1) {
    // This is (n,p); reset and calculate as if for (p,n)
    y = 0.0; iy = 0; x = 1.0; ix = 1;
  }

  // Reset x and y values for the data. We do this since we use the data
  // pointer to calculate the cross-sections in member functions. We will
  // reset x and y to the original values once we're done.
  data->setXandY(x, y);
  int ichg = data->getichg();

  // (n,xn) is like (p, p(x-1)n), both have iy=1, ix=x>=2
  // Integer to specify type of ypxn reaction, e.g. (p,2p) = 20
  // Retrieve ichg since x or y may have changed.

  if (ix <= 4 && iy == 0) {

    _sigma = this->calcxnSigma(data);

  } else if (ichg >= 11 && ichg <= 19) {

    _sigma = this->calcpxnSigma(data);    

  } else if (ichg == 20) {

    _sigma = this->calc2pSigma(data);

  } else if (ichg > 20 && ichg <= 29) {

    _sigma = this->calc2pxnSigma(data);

  } else if (ichg >= 30 && ichg < 60) {

    _sigma = this->calc3pxnSigma(data);

    // Correction for region boundaries for neutron-rich heavy targets 
    // (from ST'85, Eq 15-18). This is not inside the calc3pxnSigma function
    // since the correction method needs to call the same function (otherwise
    // we would get into a recursive loop...).
    this->apply3pxnCorrection(data, _sigma);

  } else {

    _sigma = 0.0;
    if (_debug == 1) {cout<<"sig7 = "<<_sigma<<endl;}

  }

  this->applySigmaCorrections(data);

  // Any further general updates
  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  // reset x and y to the original values
  data->setXandY(x0, y0);

  return _sigma;

}

void ActSTPeripheral::applySigmaCorrections(ActNucleiData* data) {

  if (data == 0) {return;}

  // Update 11 from ST'98: reduction in sigma for A=180-200
  int ia = data->getia();
  double at = data->getat();

  if (ia >= 180 && ia <= 200) {
    double F = 1.0 - 2e-3*(at - 100.0);
    _sigma *= F;
  }

  int izt = data->getizt();
  int iat = data->getiat();
  int ichg = data->getichg();

  // Corrections for alpha, deuteron emission and even charge from ST'85, pg 876
  if (izt >= 6 && izt <= 11) {

    int izt2 = 2*izt;
    if ((izt == 6 || izt == 8) && (iat == izt2)) {

      // C12 and O16 corrections for palpha, p2alpha and pd reactions
      if (ichg == 32 || ichg == 54 || ichg == 21) {_sigma *= 1.8;}

    } else if (izt == 10 && iat == 20) {
      // Na20 corrections for palpha, p2alpha, pd and palphad
      if (ichg == 32 || ichg == 54 || ichg == 21 || ichg == 43) {
	_sigma *= 1.8;
      } else if (ichg == 31) {
	_sigma *= 0.6; // 2pd reaction
      }

    } else if ((izt == 7 || izt == 9 || izt == 11)) {

      if (iat == izt2) {
	// Corrections for xalpha+d nuclei, x = integer; i.e. N14, F18 and Na22.
	if (ichg == 21) {
	  _sigma *= 1.8; // pd
	} else if (ichg == 11 || ichg == 20) {
	  _sigma *= 0.6; // pn and 2p
	}
      } else if (izt == izt2-1) {
	// Corrections for xalpha+p nuclei, x = integer; i.e. N13, F17 and Na21.
	if (ichg == 20) {
	  _sigma *= 1.8; // 2p
	} else if (ichg == 11) {
	  _sigma *= 0.6; // pn
	}
      }

    } else if ((izt == 6 || izt == 8 || izt == 10) && (iat == izt2+1)) {
      // Corrections for xalpha+n nuclei, x = integer; i.e. C13, O17, Ne21
      if (ichg == 11) {
	_sigma *= 1.8; // pn
      } else if (ichg == 20) {
	_sigma *= 0.6; // 2p
      }
    }
  }

  // Enhancements for some reactions from ST'98 (update 9)
  if ((izt == 12 || izt == 14 || izt == 16) && (iat == 2*izt)) {
    if (ichg == 32 || ichg == 20) {
      _sigma *= 1.6; // p alpha, 2p
    } else if (ichg == 21) {
      _sigma *= 1.8; // pd
    }
  }
}

double ActSTPeripheral::calcxnSigma(ActNucleiData* data) {

  if (data == 0) {return 0.0;}

  // (p, xn) ST'77
  // sig = sige0*G(e)*H(zt,x)*Omega
  double at = data->getat();
  double zt = data->getzt();
  double a = data->geta();
  double z = data->getz();

  int ia = data->getia();
  int iz = data->getiz();
  int izt = data->getizt();

  double e = data->gete();
  double x = data->getx();

  double e0pxn = 300.0*x + 500.0;
  double nzt(0.0), comb(0.0);
  if (fabs(zt) >= 1e-30) {
    nzt = (at - zt)/zt;
    comb = ((1.0 - x)/zt) + nzt;
  }
  double comb2 = _formulae.power((1.5 - comb), 5.0);
  double dummysig = 0.5*_formulae.expfun(-90.0*comb2);
  
  // sige0 first
  double sige0(0.0);
  nzt = fmin(1.5, nzt);
  
  if (nzt <= 1.5 && nzt >= 1.28) {
    double upt1 = (1.5 - nzt)/0.22;
    double upt2 = (nzt - 1.28)/0.22;
    double pxnsig1 = this->pxnsig(x, 1.28);
    double pxnsig2 = this->pxnsig(x, 1.5);
    sige0 = _formulae.power(pxnsig1, upt1)*_formulae.power(pxnsig2, upt2);
  } else {
    sige0 = this->pxnsig(x, nzt);
  }
  if (comb <= 1.2) {sige0 = fmin(dummysig, sige0);}
  
  // Energy dependence now
  double bigg(0.0);
  if (e >= e0pxn) {
    bigg = this->pxng(at, e0pxn, x);
  } else {
    double df = this->pxnf(at, zt, a, z, e, x);
    double dg = this->pxng(at, e, x);
    double dh = this->pxnh(at, zt, a, z, e, x);
    if (_debug == 1) {cout<<"df,dg,dh = "<<df<<" "<<dg<<" "<<dh<<endl;}
    bigg = df*(dg + dh);
  }
  
  // bigh factor now and special omega
  double bigh(1.0);
  if (izt >= 83) {
    // For heavy fissionable nuclei
    double pow1 = (zt - 80.0)/12.0;
    double pow2 = _formulae.power(pow1, 5.0);
    bigh = _formulae.expfun(0.4 - x*pow2);
    bigh = fmin(1.0, bigh);
  }
  double omega(1.0);
  if (ia == 9 && iz == 4) {
    omega = 0.65;
  } else if (ia == 13 && iz == 7) {
    omega = 0.4;
  }
  
  double sigma = sige0*bigg*bigh*omega;

  if (_debug == 1) {
    cout<<"sig1 = "<<_sigma<<endl;
    cout<<"values are "<<sige0<<" "<<bigg<<" "<<bigh<<" "<<omega<<endl;
  }

  return sigma;

}

double ActSTPeripheral::calcpxnSigma(ActNucleiData* data) {

  if (data == 0) {return 0.0;}

  double at = data->getat();
  double atbar = data->getatbar();
  double zt = data->getzt();
  double a = data->geta();

  int iat = data->getiat();

  double e = data->gete();
  double x = data->getx();
  int ix = data->getix();
  int ichg = data->getichg();

  double sigma(0.0), sige0(0.0);

  // ichg = min(9, ix) + 10*iy, e.g. (pn) = 1+10 = 11
  // (p, pxn) x>=1, eqns 24, 28-29
  if (iat <= 40) {
    sige0 = 24.0*(1.0 + 0.01*at);
  } else if (iat >= 41 && iat < 63) {
    sige0 = 1.02*(at - 7.0);
  } else if (iat >= 63) {
    sige0 = 57.0;
  }
  
  if (ichg == 11) {
    // p,pn only: eqn 25
    
    // Calculate H(E)
    double pow1 = e/25.0;
    double pow2 = _formulae.power(pow1, 4.0);
    double hofe = 1.0 - _formulae.expfun(-pow2);
    double pow3 = _formulae.power(e*0.01, 2.0);
    double pow4 = e/350.0;
    hofe *= (1.0 + 2.1*_formulae.expfun(-pow3) + 0.4*_formulae.expfun(-pow4));
    
    // Set parameter d of p 357
    double d = 3.0;
    double atmin = 35.0;
    double yieldVal = this->yield(d, at, atbar, atmin, zt);
    // Update from ST'85: phi = 1 only for p,pn
    sigma = sige0*hofe*yieldVal;
    
    // Update ST'98: enhancement by electromagnetic dissociation near 10 GeV
    if (fabs(e - 10.0e3) < 0.1) {
      double zt80 = zt/80.0;
      double F = 0.15*zt80*zt80 + 1.0;
      sigma *= F;
    }
    
  } else {
    
    // p,pxn now: eqns 28, 29
    
    // First check whether x > x_max for some cases. 
    // Use spallation xsection in this case.
    bool gotSigma(false);
    
    if (iat > 70) {
      
      double pow1 = (209.0 - at)/139.0;
      double xmax = (at/20.0) + 1.5*(pow1*pow1) + 0.8;
      int ixmax = (int) xmax;
      if (ix > ixmax) {
	// Use spallation cross-section
	sigma = this->calcSpalLiteSigma(data);
	gotSigma = true;
      }
      
    } else if (iat <= 70) {
      
      double xmax = (at/66.0) + 3.0;
      int ixmax = (int) xmax;
      if (ix > ixmax) {
	// Use spallation or light cross-section
	sigma = this->calcSpalLiteSigma(data);
	gotSigma = true;
      }
    }
    
    if (gotSigma == false) {
      
      double at1 = 0.0048*at;
      double cVal(1.5); // update ST'90
      double exp1 = _formulae.power(x, (cVal - at1));
      sige0 *= _formulae.expfun(1.0 - exp1);
      
      double hofe = 1.0 + this->h1(at, a, e) - 
	this->h2(at, a, e);
      hofe *= this->h3(at, a, e)*this->h4(e);
      double d = 15.0;
      
      double atmin = 35.0;
      double yieldVal = this->yield(d, at, atbar, atmin, zt);
      double phiVal = _formulae.phi(at, e);
      sigma = sige0*hofe*yieldVal*phiVal;
      
    }
    
  }
  
  if (_debug == 1) {cout<<"sig2 = "<<_sigma<<endl;}
  
  return sigma;

}

double ActSTPeripheral::calc2pSigma(ActNucleiData* data) {

  if (data == 0) {return 0.0;}

  // (p,2p)
  // Update from ST'85, pg 876 & 877
  double at = data->getat();
  double atbar = data->getatbar();
  double zt = data->getzt();

  int izt = data->getizt();
  int iz = data->getiz();

  double e = data->gete();

  double sigma(0.0), sige0(0.0);

  if (izt <= 5) {

    sige0 = 21.0;
    // Extra correction for low p/n ratio (ST'85, pg 876)
    double nt = data->getnt();
    if (nt > 0.0) {
      double nRatio = zt/nt;
      if (nRatio < 1.0 && fabs(nt - 2.0) > 1e-6) {
	sige0 *= (zt - 2.0)/(nt - 2.0);
      }
    }

  } else if (izt >= 6 && izt <= 23) {
    sige0 = 28.0;
  } else if (izt >= 24 && izt <= 42) {
    sige0 = 42.0;
  } else if (izt >= 43 && izt <= 83) {
    sige0 = 17.0;
  }
  
  double exp1 = _formulae.power((e/25.0), 4.0);
  double hofe = 1.0 - _formulae.expfun(-exp1);
  double exp2 = _formulae.power((e/230.0), 2.0);
  double exp3 = _formulae.power(((e-900.0)/500.0), 2.0);
  double hsum = 2.2*_formulae.expfun(-e/75.0) + 1.0;
  hsum += 0.33*_formulae.expfun(-exp3);
  hsum -= _formulae.expfun(-exp2);
  hofe *= hsum;
  
  // Set parameter d of p 357
  double d = -3.0;
  double atmin = 35.0;
  
  // Calculate sigma    
  double yieldVal = this->yield(d, at, atbar, atmin, zt);
  double phiVal = _formulae.phi(at, e);
  sigma = sige0*hofe*yieldVal*phiVal;
  
  // Update from ST'90, p 369
  if (iz == 5) {
    sigma *= 0.6;
  } else if (iz == 4) {
    sigma *= 0.5;
  } else if (iz == 3) {
    sigma /= 3.0;
  }
  
  if (_debug == 1) {
    cout<<"sige0,hofe,yield,phi = "<<sige0<<" "<<hofe<<" "<<yieldVal<<" "<<phiVal<<endl;
    cout<<"sig3 = "<<sigma<<endl;
  }

  return sigma;

}

double ActSTPeripheral::calc2pxnSigma(ActNucleiData* data) {

  if (data == 0) {return 0.0;}

  // (p, 2pxn) Eqns 31,29

  double at = data->getat();
  double atbar = data->getatbar();
  double zt = data->getzt();
  double a = data->geta();

  int izt = data->getizt();

  double e = data->gete();
  int ix = data->getix();
  int ichg = data->getichg();

  double sigma(0.0), sige0(0.0);

  // ST'90 update
  if (izt <= 50) {
    sige0 = 27.0;
  } else {
    sige0 = 21.0;
  }
  
  // First calculate xmax
  double exp1 = _formulae.power(((at - 39.0)/54.5), 2.0);
  double xmax(0.0);
  if (izt >= 35 && izt <= 40) {
    xmax = 1.0; // Update from ST'90
  } else {
    xmax = 10.0*(1.0 - _formulae.expfun(-exp1)) + 1.0;
    xmax = fmax(2.0, xmax);
  }
  
  int ixmax = (int) xmax;
  if (ix > ixmax) {

    // Use spallation or light sigma
    sigma = this->calcSpalLiteSigma(data);

  } else {
    
    // No value for d is given for p,2pxn, so I use 0
    double d = 0.0;
    double atmin = 35.0;
    if (ichg == 21) {d = -1.0;} // for (p, 2pn) case only
    
    double hofe = this->h1(at, a, e) - this->h2(at, a, e) + 1.0;
    hofe *= (this->h3(at, a, e)*this->h4(e));
    
    // Calculate sigma
    double yieldVal = this->yield(d, at, atbar, atmin, zt);
    double phiVal = _formulae.phi(at, e);
    sigma = sige0*hofe*yieldVal*phiVal;
  }

  if (_debug == 1) {cout<<"sig4 = "<<sigma<<endl;}

  return sigma;

}

double ActSTPeripheral::calc3pxnSigma(ActNucleiData* data) {

  if (data == 0) {return 0.0;}

  double at = data->getat();
  double atbar = data->getatbar();
  double zt = data->getzt();
  double a = data->geta();

  int izt = data->getizt();
  int iat = data->getiat();

  double e = data->gete();
  double ezero = data->getezero();

  double x = data->getx();
  int ix = data->getix();
  int ichg = data->getichg();

  double sigma(0.0);

  // (p,ypxn) y>=3, x>=0
  // Discussion from p 359: I use 36a for p,3pxn with
  // x>=0 and at<35; use 33a,b for p,3pxn x>=0 for heavier tgts.         
  // Use crude estimate of last lines p.359 for large y reactions.
  // First do lightest nuclei (eqn. 36). Note that xmax for       
  // these is set to 0.0 in ActSTSelect, so only (p,3p)
  // should be involved.
  
  // Set ihiy flag to denote 4pxn and 5pxn reactions, and adjust the
  // values of ichg accordingly so that we can obtain the 3pxn xsection.
  // Scaling factors for the 4p and 5p reactions are applied later on.
  int ihiy = 0;
  int oldichg(ichg);
  if (ichg >= 40 && ichg < 50) {
    ichg -= 10;
    ihiy = 1;
  } else if (ichg >= 50 && ichg <= 59) {
    ichg -= 20;
    ihiy = 2;
  }
  data->setichg(ichg);

  // For light targets
  if (izt > 5 && iat < 35) {
    
    double sigmas = this->calcSpalLiteSigma(data);
    if (ihiy == 0) {
      sigma = 0.0022*a*a*sigmas; // 3pxn sigma
    } else if (ihiy == 1) {
      sigma = 0.0022*a*a*sigmas;
      sigma = fmin(_sigma, 0.5); // 4pxn sigma <= 0.5
    } else {
      sigma = 0.0; // 5pxn sigma = 0
    }

    // Update from ST'85, p 877
    if (izt <= 7 && ix == 0) {sigma *= 1.5;}
    
  } else {
    
    // First work out if x > xmax. If so, use spallation (or light) xsection
    bool gotSigma(false);
    
    double xmax(0.0);
    if (iat > 70 && iat <= 209) {xmax = at*0.04 + 0.5;}
    int ixmax = (int) xmax;
    if (ix > ixmax) {
      sigma = this->calcSpalLiteSigma(data);
      gotSigma = true;
    }
    
    // Use the ypxn formulae
    if (gotSigma == false) {
      
      double sige0(0.0), hofe(0.0), d(0.0);
      
      // Need to use another ActNucleiData object, since the value of e = ezero      
      data->setEnergy(ezero); // Set the energy to E0
      sige0 = this->calcSpalLiteSigma(data);
      data->setEnergy(e); // reset the original energy
      
      // Next do medium masses 35 <= A <= 70 (Eqn 33a, 34)
      // H(E) from Eqn 34a and 34b and d (for yield Y calculation, Eqn 22)
      if (iat >= 35 && iat <= 70) {
	
	double exp1 = _formulae.power(e/35.0, 4.0);
	hofe = fmin((e/200.0), 1.0);
	hofe *= (1.0 - _formulae.expfun(-exp1));
	d = 0.0; // d is zero for this range of At
	
      } else if (iat > 70 && iat <= 209) {
	
	// Updates from ST'90
	if (ix <= 2 && a > 0.0) {
	  sige0 = 1.2e5*0.12*_formulae.expfun(x/0.85)/_formulae.power(a, 2.2);
	} else if (ix >= 3 && a > 0.0) {
	  sige0 = 1.2e5*_formulae.expfun((x - 2.0)*0.5)/_formulae.power(a, 2.2);
	}
	
	// Now work on H(E) eqns 34a,b
	hofe = fmin((e/200.0), 1.0);
	
	double denom = 10.0*(at - a) + 15.0;
	double eratio(0.0);
	if (fabs(denom) > 1e-30) {
	  eratio = e/denom;
	}
	double exp1 = _formulae.power(eratio, 4.0);
	hofe *= (1.0 - _formulae.expfun(-exp1));
	
	// Extra terms for x <= 4
	if (x <= 4) {
	  
	  double xpont = 0.72 - 0.18*x;
	  hofe *= _formulae.power((e/420.0), xpont);
	  hofe = fmin(1.0, hofe);
	  
	} // x <= 4
	
	if (ichg == 31) {
	  d = -3.0;  // 3pn
	} else if (ichg == 30) {
	  d = -10.0; // 3p
	}
	
      } // 70 < at <= 209
      
      double atmin(70.0), factor(1.0);
      
      // Reduction factors for 4pnx (ihiy = 1) and 5pxn (ihiy = 2) reactions
      // ihiy is calculated above
      if (ihiy == 1) {      
	if (e >= 2000.0) {
	  factor = 0.1;
	} else {
	  factor = 0.0;
	}
      } else if (ihiy == 2) {
	if (e >= 2000.0) {
	  factor = 0.01;
	} else {
	  factor = 0.0;
	}
      }
      
      double yieldVal = this->yield(d, at, atbar, atmin, zt);
      double phiVal = _formulae.phi(at, e);
      sigma = factor*sige0*hofe*yieldVal*phiVal;
      
      // Corrections from ST'85, p877 to reduce some yields for 3pxn reactions
      if (iat >= 35 && iat <= 70) {
	if (ix == 1) {
	  sigma = _formulae.power(sigma*sige0, 0.5);
	} else if (ix == 2) {
	  sigma = _formulae.power(sigma, 0.75)*_formulae.power(sige0, 0.25);
	}
      }

      if (_debug == 1) {
	cout<<"sig5 = "<<_sigma<<endl;
	cout<<sige0<<" "<<hofe<<" "<<yieldVal<<" "<<phiVal<<endl;
      }
      
    } // ypxn formulae
  } // non-light nuclei
  
  // Reset ichg integer (for the case 4pxn and 5pxn).
  data->setichg(oldichg);

  return sigma;

}

void ActSTPeripheral::apply3pxnCorrection(ActNucleiData* data, double& sigma) {

  // Correction for region boundaries for neutron-rich heavy targets 
  // (from ST'85, Eq 15-18) for xmax < x < x0 and At > 70. 
  // Note that x > xmax uses spallation formulae. This function modifies
  // the spallation sigma depending on the value of x0 = At - A0 - 2 (Eq 15).

  if (data == 0) {return;}

  int iat = data->getiat();
  
  if (iat > 70 && iat <= 209) {

    double at = data->getat();
    double ix = data->getix();

    double xmax = at*0.04 + 0.5;
    int ixmax = (int) xmax;

    if (ix > ixmax) {
 
      double z = data->getz();
      double zt = data->getzt();
      double atbar = data->getatbar();
      double x = data->getx();

      // Calculate A0 (Eq 16)
      double s(0.0);
      if (zt > 0.0) {s = -0.07*(at - atbar)/zt; s += 0.482;}
      double t(0.00028);
      double A0Term = s*s - 4.0*t*z;
      double A0(0.0);
      ActSTSpallation spallation("spal", false, false);

      if (A0Term > 0.0 && fabs(t) > 1e-30) {
	A0 = (s - sqrt(A0Term))/(2.0*t);
	int iz = data->getiz();
	int izt = data->getizt();
	double m = spallation.calcMValue("spal", izt, iz);
	A0 -= m;
      }

      double x0 = (at - A0 - 2.0);
      if (x < x0) {

	double ezero = data->getezero();
	double energy = data->gete();

	// Apply interpolation.
	// First calculate the p factor for Eq 18.
	double P = spallation.calcPValue(data);
	// Calculate sig0 (for Eq 18) at E = E0
	double dela0(0.0);
	data->setEnergy(ezero);
	double sig0 = spallation.calcSpallSigma(data, dela0);
	data->setEnergy(energy); // reset the energy in the data object
	  
	// Eq 18 in ST'85
	double sig_x0 = sig0*_formulae.expfun(-P*(at - A0));
	  
	// Need sigma at xmax; At is the same as before, since 
	// xmax was calculated from At.
	// Use the 3pxn formulae (Eq 21, 34 and 35 from ST'73 II)
	double sig_xmax = this->calc3pxnSigma(data);

	double xRatio(0.0);
	double x0Diff = x0 - xmax;
	if (fabs(x0Diff) > 1e-6) {xRatio = (x - xmax)/x0Diff;}
	
	// Calculate final sigma value (Eq 17)
	sigma = (sig_x0 - sig_xmax)*xRatio + sig_xmax;

      } // x < x0
	
    } // ix > ixmax

  } // iat > 70

}

// Helper functions

double ActSTPeripheral::pxnsig(double x, double nzt) {

  double absTerm = fabs(6.9 - x)/4.5;
  double exp1 = _formulae.power(absTerm, 2.8);
  double exp2 = _formulae.power(x, 2.35);
  double exp3 = _formulae.power((1.5 - nzt), 5.0);

  double pxnsig = 3.3*_formulae.expfun(-(90.0*exp2*exp3 + exp1));

  return pxnsig;

}

double ActSTPeripheral::pxng(double at, double e, double x) {

  double f1 = 0.01*(at - 208.0);
  f1 = fmax(0.0, f1);

  double exp1 = 1.3 - f1;
  double inv20e(0.0);
  if (fabs(e) > 1e-30) {inv20e = 20.0/e;}
  double denom = inv20e*_formulae.power(x, 1.5) + e;
  double term(0.0);
  if (fabs(denom) > 1e-30) {term = 1300.0/denom;}

  double pxng = _formulae.power(term, exp1);

  return pxng;

}

double ActSTPeripheral::pxnh(double at, double zt, double ap, 
			     double zp, double e, double x) {

  double n = ap - zp;
  double nt = at - zt;
  double f2(1.0), f3(0.0);
  int izt = (int) zt;
  int intval = (int) nt;
  int in = (int) n;

  if (izt > 83) {
    double at208 = at - 208.0;
    f2 = 0.03*x*at208 + 1.0;
    f2 = fmax(1.0, f2);
    f3 = 0.03*(x - 1.0)*at208;
    f3 = fmax(0.0, f3);
  }

  double invsqrtx(0.0);
  if (x > 1.0e-30) {invsqrtx = 1.0/sqrt(x);}
  double term = (200.0 - at)*invsqrtx + at;
  double d = 5.0*_formulae.power(x, 1.34) - 0.1*term + 27.5;

  double denom = -2.5*_formulae.power(x, -4.0) + 6.0;
  double dummy(0.0);

  double num = e - d + f3;

  // App B note
  int ix = (int) x;
  if (ix >= 3) {
    if (izt >= 39 && intval >= 50 && in < 50) {
      num -= 3.0;
    } else if (izt >= 59 && intval >= 82 && in < 82) {
      num -= 3.0;
    }
  } else if (ix == 1) {
    if (izt >= 79 && izt <= 83) {
      num += 5.0;
    }
  }

  if (fabs(denom) >= 1e-30) {
    dummy = num/denom;
    dummy *= dummy;
    dummy = 0.6*x + 0.5*dummy;
  }

  double denom2 = -_formulae.power(x, -4.0) + 2.0;
  double dummy2(0.0);

  if (fabs(denom2) > 1e-30) {
    double term2 = 0.03*at/denom2;
    dummy2 = _formulae.power(term2, 3.0);
  }

  // f2 >= 1
  double pxnh = 3500.0*(1.0 - _formulae.expfun(-dummy2))*_formulae.expfun(-dummy)/f2; 

  return pxnh;

}

double ActSTPeripheral::pxnf(double at, double zt, double ap, 
			     double zp, double e, double x) {

  double pxnf(0.0);

  double n = ap - zp;
  double nt = at - zt;
  double atd = fmax(35.0, at);
  double dummy = _formulae.power(atd, (2.0/3.0));
  dummy *= (1.0 - _formulae.power(x, -0.5));
  dummy = (0.1*x + 12.0)*x - dummy;

  double invDummy(0.0);
  if (fabs(dummy) > 1e-30) {invDummy = 1.0/dummy;}
  double power = 1.5*x + 1.0;

  double delta(0.0);

  int izt = (int) zt;
  int in = (int) n;
  int intval = (int) nt;
  int iat = (int) at;

  // App B note
  int ix = (int) x;
  if (ix >= 3) {
    if (izt >= 39 && intval >= 50 && in < 50) {
      delta = 10.0;
    } else if (izt >= 59 && intval >= 82 && in < 82) {
      delta = 3.0;
    }
  }

  double eratio = (e - delta)*invDummy;
  double exp1 = _formulae.power(eratio, power);

  pxnf = -_formulae.expfun(-exp1) + 1.0;

  if (ix < 2) {

    // (p,n) reactions
    double decay = this->ed(zp, ap);
    if (iat <= 40 && e > decay && decay > 0.0) {
     
      double denom = 2.0*sqrt(decay);
      double eratio(0.0);
      if (denom > 1e-30) {eratio = (e - decay)/denom;}
      double exp1 = _formulae.power(-eratio, 2.5);

      pxnf = -_formulae.expfun(exp1) + 1.0;

    }
  }

  return pxnf;

}

double ActSTPeripheral::yield(double d, double at, double atbar,
			      double atmin, double zt) {

  double yield(1.0);

  if (at >= atmin && at <= 209.1) {

    double dummy = (at - atbar)*d;
    double invzt(0.0);
    if (fabs(zt) > 1e-30) {invzt = 1.0/zt;}
    
    if (dummy <= 0.0) {
      yield = _formulae.expfun(dummy*invzt);
    } else {
      yield = -_formulae.expfun(-dummy*invzt) + 2.0;
    }
  }

  return yield;

}

double ActSTPeripheral::h1(double at, double a, double e) {

  double term1 = (at - a)/7.9;
  double term2 = e/420.0;
  double expTerm = _formulae.power(term2, 1.4) + term1*term1;

  double h1 = 1.9*_formulae.expfun(-expTerm);
  return h1;

}

double ActSTPeripheral::h2(double at, double a, double e) {

  double term1 = (at - a)/12.0;
  double term2 = e/420.0;
  double power1 = _formulae.power(term1, 8.0);
  double power2 = _formulae.power(term2, 3.0);

  double h2 = (1.0 - _formulae.expfun(-power1))*_formulae.expfun(-power2);
  return h2;

}

double ActSTPeripheral::h3(double at, double a, double e) {

  double denom = 10.0*(at - a) + 15.0;
  double eratio(0.0);
  if (fabs(denom) > 1e-30) {
    eratio = e/denom;
  }

  double power = _formulae.power(eratio, 4.0);

  double h3 = -_formulae.expfun(-power) + 1.0;
  return h3;

}

double ActSTPeripheral::h4(double e) {

  double inve(0.0);
  if (fabs(e) > 1e-30) {inve = 1.0/e;}

  double h4 = 400.0*inve - 2.2;
  if (h4 > 2.0) {
    h4 = 2.0;
  } else if (h4 < 1.0) {
    h4 = 1.0;
  }

  return h4;

}

double ActSTPeripheral::ed(double zp, double ap) {

  // Decay energies of product nuclei for light targets
  // table 1 ST'77
  double ed(0.0);

  int zpi = (int) (zp);
  int api = (int) (ap);

  if (zpi == 4 && api == 6) {
    ed = 6.0;
  } else if (zpi == 4 && api == 7) {
    ed = 0.86;
  } else if (zpi == 5 && api == 9) {
    ed = 9.0;
  } else if (zpi == 5 && api == 10) {
    ed = 0.0;
  } else if (zpi == 6 && api == 10) {
    ed = 3.6;
  } else if (zpi == 6 && api == 11) {
    ed = 2.0;
  } else if (zpi == 7 && api == 12) {
    ed = 17.6;
  } else if (zpi == 7 && api == 13) {
    ed = 2.2;
  } else if (zpi == 8 && api == 14) {
    ed = 5.2;
  } else if (zpi == 8 && api == 15) {
    ed = 2.8;
  } else if (zpi == 9 && api == 16) {
    ed = 16.0;
  } else if (zpi == 9 && api == 17) {
    ed = 2.8;
  } else if (zpi == 9 && api == 18) {
    ed = 1.7;
  } else if (zpi == 10 && api == 19) {
    ed = 3.2;
  } else if (zpi == 11 && api == 20) {
    ed = 13.9;
  } else if (zpi == 11 && api == 21) {
    ed = 3.6;
  } else if (zpi == 11 && api == 22) {
    ed = 2.8;
  } else if (zpi == 11 && api == 23) {
    ed = 4.1;
  } else if (zpi == 13 && api == 24) {
    ed = 13.9;
  } else if (zpi == 13 && api == 25) {
    ed = 7.3;
  } else if (zpi == 13 && api == 26) {
    ed = 4.0;
  } else if (zpi == 14 && api == 27) {
    ed = 4.8;
  } else if (zpi == 15 && api == 28) {
    ed = 14.3;
  } else if (zpi == 15 && api == 29) {
    ed = 4.9;
  } else if (zpi == 15 && api == 30) {
    ed = 4.2;
  } else if (zpi == 16 && api == 31) {
    ed = 5.4;
  } else if (zpi == 17 && api == 32) {
    ed = 12.8;
  } else if (zpi == 17 && api == 33) {
    ed = 5.6;
  } else if (zpi == 17 && api == 34) {
    ed = 5.5;
  } else if (zpi == 17 && api == 35) {
    ed = 1.1;
  } else if (zpi == 18 && api == 35) {
    ed = 6.0;
  } else if (zpi == 18 && api == 37) {
    ed = 0.8;
  } else if (zpi == 19 && api == 36) {
    ed = 12.9;
  } else if (zpi == 19 && api == 37) {
    ed = 6.1;
  } else if (zpi == 19 && api == 38) {
    ed = 0.6;
  } else if (zpi == 19 && api == 40) {
    ed = 1.3;
  } else if (zpi == 20 && api == 39) {
    ed = 6.5;
  } else if (zpi == 20 && api == 40) {
    ed = 0.0;
  } else if (zpi == 21 && api == 40) {
    ed = 14.3;
  }

  return ed;
  
}

std::string ActSTPeripheral::getLiteName(int iz, int ia, int izt) {

  std::string name("prph");

  if (iz >= 2 && iz <= 4 && ia >= 6 && ia <= 12) {
    name = "lit1";
  } else if (iz >= 6) {
    if (izt >= 6 && izt <= 16) {
      name = "lit2";
    } else if (izt >= 17 && izt <= 20) {
      name = "lit3";
    } else if (izt >= 21 && izt <= 28) {
      name = "lit4";
    }
  }

  return name;

}

double ActSTPeripheral::calcSpalLiteSigma(ActNucleiData* data) {

  // Calculate the spallation (or light product) cross-section
  // when we have x > xmax, for example.
  double sigma(0.0);
  if (data == 0) {return 0.0;}

  int izt = data->getizt();

  if (izt < 28) {

    // Light products
    int iz = data->getiz();
    int ia = data->getia();
    std::string sLiteName = this->getLiteName(iz, ia, izt);
    ActSTSLite sLite(sLiteName, false, _debug);
    sigma = sLite.calcCrossSection(data);

  } else {

    ActSTSpallation spallation("spal", false, _debug);
    sigma = spallation.calcCrossSection(data);

  }

  return sigma;

}
