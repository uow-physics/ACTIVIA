// Calculate the tritium cross-section.
// Ref.: A.Yu. Konobeyev and Yu.A. Korovin, NIM B82 (1993) 103-115

#include "Activia/ActTritiumModel.hh"
#include "Activia/ActFormulae.hh"
#include "Activia/ActNucleiData.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActTritiumModel::ActTritiumModel(std::string name, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
}

ActTritiumModel::~ActTritiumModel() 
{
  // Destructor
}

double ActTritiumModel::calcCrossSection(ActNucleiData* data) {
  
  if (data == 0) {
    cout<<"Error in ActTritiumModel. Data object is null"<<endl;
    return 0.0;
  }

  _sigma = 0.0;

  double edummy(600.0), ec(8.0), atrit(3.0);
  double n0(0.024), r0(1.0), aeps(-0.03026);
  double beps(21.46), av(0.1), bv(0.8);
  double q2(0.63), q3(16.0), p1(-0.03364);
  double p2(5.663);

  double zt = data->getzt();
  double e = data->gete();
  double at = data->getat();
  double anat(at);
  double logat(0.0); 
  if (at > 0.0) {logat = log(at);}

  double btrit = av*zt + bv; // Eq 13
  double mua = 0.1*logat + 0.2; // Eq 14c
  double etaea = 1.8*_formulae.power(e, mua); // Eq 14b
  double etaeadum = 1.8*_formulae.power(edummy, mua);

  double atec = at*ec;
  double exp1(0.0);
  if (std::fabs(etaea) > 1e-30) {exp1 = atec/(3.0*etaea);}
  double estar = etaea*(1.0 - _formulae.expfun(-exp1)); // Eq 14a

  double exp2(0.0);
  if (std::fabs(etaeadum) > 1e-30) {exp2 = atec/(3.0*etaeadum);}
  double estardum = etaeadum*(1.0 - _formulae.expfun(-exp2));

  double ratio1(0.0), ratio2(0.0);
  if (std::fabs(at) > 1e-30) {
    ratio1 = estar/at; ratio2 = estardum/at;
  }
  // Nuclear temperature
  double te(0.0);
  if (ratio1 > 0.0) {te = sqrt(8.0*ratio1);} // Eq 8
  double tdum(0.0);
  if (ratio2 > 0.0) {tdum = sqrt(8.0*ratio2);}

  double r0at = r0*at/105.0;
  double rt = r0at*_formulae.expfun(-0.002*e - 20.0) + 1.0; // Eq 4
  double rtdum = r0at*_formulae.expfun(-0.002*edummy - 20.0) + 1.0;
  
  double zetaa = 0.087*_formulae.power(at, (2.0/3.0)) + 4.15; // eq 5b
  double lambdaa = 0.72*_formulae.power((1.0 + logat), -0.4); // Eq 5c

  double sqrtatp1 = sqrt(at) + 1.0;
  double log368 = log(3.68);
  double loge(0.0);
  if (e > 0.0) {loge = log(e);}
  // Eq 5a
  double ncasc = sqrtatp1*_formulae.expfun(-zetaa*_formulae.expfun(-lambdaa*(loge - log368)));
  double loged(0.0);
  if (std::fabs(edummy) > 1e-30) {loged = log(edummy);}
  double ncascdum = sqrtatp1*_formulae.expfun(-zetaa*_formulae.expfun(-lambdaa*(loged - log368)));

  double epseps = aeps*zt + beps; // Eq 6
  double sigmaeval = q2*anat + q3 + (p1*zt + p2)*(at - anat); // Eq 17 (q1 = 0)

  // Obtain the threshold energy
  double st(0.0);

  int izt = data->getizt();
  int iat = data->getiat();
  if (izt >= 13 && izt <= 83) {

    double ztmod = fmod(zt, 2.0);

    // S_t from Eq 10
    if (ztmod < 0.1) {
      st = 9.48e-4*at + 14.33; // even Z
    } else if (ztmod > 0.1) {
      st = 9.55e-3*at + 12.68; // odd Z
    }

    if (izt == 83 && iat == 209) {st = 14.279;}

  } else if (izt >= 6 && iat <= 12) {

    if (_debug) {
      cout<<"No tritium production on target ("<<izt<<", "<<iat<<")."<<endl;
    }
    return 0.0;

  } else {

    if (_debug) {
      cout<<"No tritium production on target ("<<izt<<", "<<iat<<")."<<endl;
    }
    return 0.0;

  }

  if (e < st) {return 0.0;}

  // Calculate G_th, Eq 7
  double estte(0.0);
  if (std::fabs(te) > 1e-30) {estte = (e - st)/te;}
  double gth = 1.0 - (1.0 + estte)*_formulae.expfun(-estte);

  double esttedum(0.0);
  if (std::fabs(tdum) > 1e-30) {esttedum = (edummy - st)/tdum;}
  double gthdum = 1.0 - (1.0 + esttedum)*_formulae.expfun(-esttedum);

  // Work out Eq 2 for E = 600 MeV
  double expeps(0.06*epseps);
  double n0atr(0.0);
  if (std::fabs(atrit) > 1e-30) {n0atr = n0*(at/atrit);}
  double fpre600(0.0);
  if (std::fabs(at) > 1e-30) {
    fpre600 = n0atr*rtdum*gthdum*_formulae.power((ncascdum/at), expeps);
  }

  double tdumec = tdum + ec;
  // Work out Eq 3
  double feq600(0.0);
  if (std::fabs(tdum) > 1e-30 && std::fabs(tdumec) > 1e-30) {
    feq600 = _formulae.expfun(-btrit/tdum)*gthdum*estardum/tdumec;
  }

  // Calculate sigma_non
  double sn = this->sigmanon(data);

  double w(0.0);
  if (std::fabs(sn) > 1e-30 && std::fabs(feq600) > 1e-30) {
    w = ((sigmaeval/sn) - fpre600)/feq600; // Eq 16
  }

  // Eq 2 for energy E
  double fpre(0.0);
  if (std::fabs(at) > 1e-30) {
    fpre = n0atr*rt*gth*_formulae.power((ncasc/at), expeps);
  }

  // Eq 3 for energy E
  double teec = te + ec;
  double feq(0.0);
  if (std::fabs(te) > 1e-30 && std::fabs(teec) > 1e-30) {
    feq = _formulae.expfun(-btrit/te)*gth*estar/teec;
  }

  // Eq 1
  _sigma = sn*(fpre + w*feq);

  return _sigma;

}

double ActTritiumModel::sigmanon(ActNucleiData* data) {

  double sn(1.0);
  if (data == 0) {return sn;}

  double e = data->gete();
  double zt = data->getzt();
  int izt = data->getizt();

  double x = e/600.0;

  if (izt > 28) {

    double log10x = log10(x);

    if (x < 1.0) {
   
      double ztr = (zt - 50.0)/28.0;      
      double term1 = 3.536*_formulae.expfun(1.6126*ztr)*log10x;
      double term2 = 0.53*_formulae.expfun(2.072*ztr);
      sn = (term1 + term2)*log10x + 1.0;

    } else {

      sn = 1.0 - (0.095*zt*log10x/28.0);

    }
  }

  return sn;

}

