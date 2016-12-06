// Class to calculate common formulae

#include "Activia/ActFormulae.hh"

#include <cmath>

ActFormulae::ActFormulae()
{
  // Constructor

}

ActFormulae::~ActFormulae() 
{
  // Destructor
}

double ActFormulae::nzstar(double zp, double ap, double zt,
			   double at, double atbar) {

  double np = ap - zp;
  double ratio1(0.0);
  if (std::fabs(zp) > 1e-30) {ratio1 = np/zp;}
  double ratio2(0.0);
  if (std::fabs(zt) > 1e-30) {ratio2 = (at - atbar)/zt;}
  double nzstar = -0.3*ratio2 + ratio1;

  return nzstar;

}

double ActFormulae::bigg(double z, double a, double zt, 
			 double at, double atbar, double e) {

  // Calculate complicated energy dependence factor - eqn 15
  double ac = a;
  double zc = z;
  double nvrz(0.0), nvrzt(0.0);
  if (std::fabs(z) > 1e-30 && std::fabs(zt) > 1e-30) {
    nvrz = (a/z) - 1.0;
    nvrzt = (at/zt) - 1.0;
  }
  if (nvrz > nvrzt) {ac = at; zc = zt;}

  double bigg(1.0);

  if (a >= 20.0 && a <= 30.0) {

    double nzstarVal = this->nzstar(zc, ac, zt, at, atbar);
    double expTerm = 0.01*at + nzstarVal - 3.1;
    expTerm *= 2600.0/fmax(e, 500.0);
    bigg = this->expfun(expTerm);

  } else if (a >= 31.0 && a <= 56.0) {

    double nzstarVal = this->nzstar(zc, ac, zt, at, atbar);
    double expTerm = 0.0035*at + nzstarVal - 1.72;
    expTerm *= 5600.0/fmax(e, 500.0);
    bigg = this->expfun(expTerm);

  }

  if (bigg < 1.0) {bigg = 1.0;}
  return bigg;

}

double ActFormulae::fbe(double z, double a, double zt, double at, 
			double atbar, double e, double ezero) {

  double fbe(0.0);

  if (z < 11.0) {

    double alpha = 1.8;

    if (z >= 8.999) {alpha = 2.0;}
    double eratio(0.0);
    if (std::fabs(ezero) > 1e-30) {eratio = e/ezero;}
    fbe = this->power(eratio, alpha);

    return fbe;

  }

  double upsln = this->power((at/63.0), 4.0);
  double ezero03 = 0.3*ezero;

  if (e < ezero03) {

    double biggVal = this->bigg(z, a, zt, at, atbar, e);
    double power1 = -0.01*upsln + 2.2;
    double power2 = 0.02*upsln;
    double ratio1(0.0), ratio2(0.0);

    if (std::fabs(ezero) > 1e-30) {
      ratio1 = e/ezero; ratio2 = e/ezero03;
    }
    double term1 = this->power(ratio1, power1);
    double term2 = this->power(ratio2, power2);

    fbe = biggVal*term1*term2;

  } else if (e >= ezero03 && e <= ezero) {

    double biggVal = this->bigg(z, a, zt, at, atbar, e);
    double power = -0.01*upsln + 2.2;
    double ratio(0.0);

    if (std::fabs(ezero) > 1e-30) {ratio = e/ezero;}

    double term = this->power(ratio, power);

    fbe = biggVal*term;

  } else if (e > ezero) {

    fbe = 1.0;

  }

  return fbe;

}

double ActFormulae::calcOmega(double zp, double ap, std::string formula) {

  // Nuclear structure factors, table 2 p 321+table 1, p874 in ST'85, p369 ST'90

  double omega(1.0);
  int zpi = (int) (zp);
  int api = (int) (ap);

  if (formula == "lit1" || formula == "lit2") {

    if (zpi == 2 && api == 6) {
      omega = 0.6; // He6
    } else if (zpi == 3) {
      if (api == 7) {
	omega = 1.8; // Li7
      } else if (api == 8) {
	omega = 0.7; // Li8
      } else if (api == 9) {
	omega = 3.0; // Li9
      }
    } else if (zpi == 4) {
      if (api == 7) {
	omega = 0.95; // Be7
      } else if (api == 9) {
	omega = 0.65; // Be9
      } else if (api == 10) {
	omega = 1.3; // Be10
      }
    } else if (zpi == 5) {
      if (api == 8) {
	omega = 0.8; // B8
      } else if (api == 10) {
	omega = 0.7; // B10
      } else if (api == 11) {
	omega = 1.7; // B11
      }
    } else if (zpi == 6 && api == 10) {
      omega = 0.6; // C10
    } else if (zpi == 7) {
      if (api == 13) {
	omega = 0.4; // N13
      } else if (api == 15) {
	omega = 2.0; // N15
      }
    } else if (zpi == 8 && api == 15) {
      omega = 1.2; // O15
    } else if (zpi == 10) {
      if (api == 19) {
	omega = 0.6; // Ne19
      } else if (api == 21) {
	omega = 0.8; // Ne22
      }
    } else if (zpi == 11 && api == 22) {
      omega = 1.2; // Na22
    }
  }

  return omega;

}

double ActFormulae::calcXi(double at, double zp, double ap) {

  // table 2, page 874 in [Sil85]

  double xi(1.0);

  int ati = (int) (at);
  int zpi = (int) (zp);
  int api = (int) (ap);

  if (ati >= 14 && ati < 34) {

    // From table 3, ST'73 I, p322
    if (zpi == 2 && api == 6) {
      xi = 0.1*(at - 14.0) + 1.0;
    }

  } else if (ati >= 34 && ati <= 63) {

    if (zpi == 2 && api == 6) {
      xi = 3.0*(1.0 + 0.02*(at - 34.0)); // He6
    } else if (zpi == 3 && api == 6) {
      xi = 1.0 + 0.02*(at - 34.0); // Li6
    } else if ((zpi == 3 && api == 7) || (zpi == 4 && api == 7)) {
      xi = 1.0 + 0.01*(at - 34.0); // Li7 and Be7
    }

  } else if (ati >= 64 && ati <= 104) {

    if (zpi == 2 && api == 6) {
      xi = 4.8 + 0.045*(at - 64.0); // He6
    } else if (zpi == 3) {
      if (api == 6) {
	xi = 1.6 + 0.015*(at - 64.0); // Li6
      } else if (api == 7) {
	xi = 1.3 + 0.015*(at - 64.0); // Li7
      } else if (api == 8) {
	xi = 1.0 + 0.0225*(at - 64.0); // Li8
      } else if (api == 9) {
	xi = 1.0 + 0.0125*(at - 64.0); // Li9
      }
    } else if (zpi == 4) {
      if (api == 7) {
	xi = 1.3 + 0.0105*(at - 64.0); // Be7
      } else if (api == 9) {
	xi = 1.0 + 0.01*(at - 64.0); // Be9
      } else if (api == 10) {
	xi = 1.0 + 0.005*(at - 64.0); // Be10
      }
    } else if (zpi == 5 && (api == 10 || api == 11)) {
      xi = 1.0 + 0.005*(at - 64.0); // B10, B11
    } 

  } else if (ati > 104) {

    if (zpi == 2 && api == 6) {
      xi = 6.6; // He6
    } else if (zpi == 3) {
      if (api == 6) {
	xi = 2.2; // Li6
      } else if (api == 7) {
	xi = 1.9; // Li7
      } else if (api == 8) {
	xi = 1.9; // Li8
      } else if (api == 9) {
	xi = 1.5; // Li9
      }
    } else if (zpi == 4) {
      if (api == 7) {
	xi = 1.7; // Be7
      } else if (api == 9) {
	xi = 1.4; // Be9
      } else if (api == 10) {
	xi = 1.2; // Be10
      }
    } else if (zpi == 5 && (api == 10 || api == 11)) {
      xi = 1.2; // B10, B11
    }   
  }

  return xi;

}

double ActFormulae::cp(double at, double zt, double e, double ezero) {

  double cp(1.0);

  if (zt >= 21.0 && zt <= 30.0) {
    double exp1 = 0.01*e - 1.0;
    cp = 1.0 - 0.32*this->expfun(-exp1*exp1);
  } 

  if (at > 100.0) {
    double ratio = (ezero + 150.0)/(e + 150.0);
    cp = 1.0 - 1.5e-5*(at - 100.0)*ratio;
  }

  return cp;

}

double ActFormulae::f1(double atgt, double e, double ezero) {

  double f1(1.05);

  if (e <= 750.0) {

    f1 = this->expfun(0.0074*atgt - 0.25);

  } else if (e < ezero && std::fabs(ezero - 750.0) > 1e-30) {

    double expTerm = this->expfun(0.0074*atgt - 0.25);
    double ratio = (ezero - e)/(ezero - 750.0);
    f1 += (expTerm - 1.05)*ratio*ratio;
    
  }

  return f1;

}

double ActFormulae::f11(double e, double ezero) {

  double ep(0.0);
  if (ezero > 1e-30) {ep = 2262.0*e/ezero;}
  if (e < ezero) {ep = e;}
  double epSq = ep*ep;

  double term1 = 0.6*(1.0 - this->expfun(-epSq*1.0e-6))*this->expfun(-epSq*2.5e-7);
  double f11 = 0.2*(1.0 - this->expfun(-epSq/9.0e6)) + 1.0 - term1;

  return f11;

}

double ActFormulae::f2(double e) {

  double f2(1.0);
  if (e < 240.0) {f2 = this->expfun(-0.0071*e + 1.73);}

  return f2;

}

double ActFormulae::f22(double e, double ezero) {

  double ep(0.0);
  if (ezero > 1e-30) {ep = 2262.0*(e/ezero);}
  double exp1 = ep*5.0e-4;
  double expTerm = -this->expfun(-exp1*exp1) + 1.0;
  double exp2 = (ep - 1800.0)/1800.0;

  double f22 = (-0.4*this->expfun(-exp2*exp2) + 0.17)*expTerm + 1.0;

  return f22;

}

double ActFormulae::f2p(double e) {

  double f2p(1.0);
  if (e < 600.0) {f2p = this->expfun(-0.0015*e + 0.9);}
  return f2p;

}

double ActFormulae::f3(double zt, double e) {

  double f3(1.0);

  if (zt > 20.0 && e < 1250.0) {

    double exp1 = (e - 650.0)/720.0;
    f3 = 2.0*this->expfun(-exp1*exp1);

  }

  return f3;

}

double ActFormulae::f33(double e, double ezero) {

  double ep(0.0);
  if (ezero > 1e-30) {ep = 2262.0*e/ezero;}
  double exp1 = ep/1500.0;
  double exp2 = (ep - 1500.0)/1800.0;
  double exp3 = ep/2000.0;

  double f33 = 0.25*(1.0 - this->expfun(-exp1*exp1))*this->expfun(-exp2*exp2);
  f33 -= 0.05*(1.0 - this->expfun(-exp3*exp3));
  f33 += 1.0;

  return f33;

}

double ActFormulae::f44(double e, double ezero) {

  double ep(0.0);
  if (ezero > 1e-30) {ep = 2262.0*e/ezero;}
  double exp1 = ep/4000.0;
  double f44 = -0.1*(1.0 - this->expfun(-exp1*exp1)) + 1.0;

  return f44;

}

double ActFormulae::dac(double at, double e, double ezero) {

  double at36 = at - 36.0;
  double dac = 0.052*at36*(log(e) - 3.17) + 31.5;
  if (e >= ezero) {
    dac = 0.045*at36*(log(at) + 1.23) + 31.5;
  }

  return dac;

}

double ActFormulae::invpat(double p, double at) {

  double invpat(0.0);
  double pat = p*at;
  double pat03(0.0);

  if (std::fabs(pat) > 1e-30) {pat03 = 0.3/pat;}

  double denom = -this->expfun(-pat)*(0.7 - pat03) + 1.0 - pat03;
  if (std::fabs(denom) > 1e-30) {invpat = 1.0/denom;}
  
  return invpat;

}

double ActFormulae::phi(double at, double e) {

  double phi(1.0);

  if (157.0 <= at && at <= 210.0) {

    if (500.0 <= e && e <= 1000.0) {
      phi = 1.0 - 0.011*(at - 157.0)*(e - 500.0)/500.0;
    } else if (e > 1000.0) {
      phi = 1.0 - 0.011*(at - 157.0);
    }
  }

  return phi;

}

double ActFormulae::fofa4(double zt, double at, double z,
			  double a, double atbar) {
  
  double lnterm = log(a/15.0);

  double nzstar = this->nzstar(z, a, zt, at, atbar);
  double nzterm = 0.0;
  double a17 = a - 17.0;
  if (a17 > 0.0) {nzterm = -sqrt(a17)/40.0;}
  double nzstar1 = nzstar - 1.0;
  nzterm += nzstar1;
  double atterm = 0.1*(at - 56.0);
  double exp1 = atterm*nzterm*lnterm;

  // Extra exponential factor from ST'90, p 369
  double fofa4 = this->expfun(exp1)*this->expfun(-2.5*nzstar1);

  if (fofa4 < 1.0) {fofa4 = 1.0;}

  return fofa4;
}

double ActFormulae::gamma(double z, double a, double zt, 
			  double at, double atbar, double e) {

  // Gamma given below from eqn 7, p 350.
  // Update from ST'85, eq 19
  double z0 = 53.0;
  int izt = (int) zt;
  if (izt >= 76 && izt <= 79) {
    z0 = 52.0;
  } else if (izt < 76) {
    z0 = 51.0;
  }

  double gam = 0.007*(z - z0) + 0.03;
  double nzstarVal = this->nzstar(z, a, zt, at, atbar);
  double eTerm = this->power(e, (2.0/3.0));
  gam *= (1.56 - nzstarVal)*eTerm;

  return gam;

}

double ActFormulae::zofa(double a) {

  // Approximate inversion of formula p.320 paper1- formula gives        
  // atbar(z), here I calculate z(at)                                    
  double zofa(a*0.5);

  if (a > 20.0) {

    double term1 = (sqrt(0.06*a + 4.0) - 2.0)/0.03;
    double powTerm = this->power((a - 10.0), 1.15)/30.0;
    zofa = term1 + powTerm;

  }

  return zofa;

}

double ActFormulae::power(double x, double n) {

  // Special power law function to check against nan values
  // First protect against 1/0^n values (infinity)
  if (std::fabs(x) < 1e-30 && n < 0.0) {return 0.0;}

  double power = pow(x, n);

  if (std::isnan(power) == 1) {
    // Try |x|^n
    power = std::pow(std::fabs(x), n);
    if (std::isnan(power) == 1) {power = 0.0;}
  }

  return power;
}

double ActFormulae::expfun(double x) {

  // Limit exponential to ~10^{+-30}
  double expVal(0.0);
  if (std::fabs(x) < 70.0) {expVal = exp(x);}

  return expVal;

}
