// Class to select the cross-section formula based on
// table 1 p.340 in Silberberg and Tsao. This does not depend
// on energy.

#include "Activia/ActSTSelect.hh"

#include <cmath>
#include <iostream>
#include <algorithm>

using std::cout;
using std::endl;

ActSTSelect::ActSTSelect(ActNucleiData& data)
{
  // Constructor
  _zt = data.getzt(); _at = data.getat(); 
  _atbar = data.getatbar(); _z = data.getz(); 
  _a = data.geta(); _e = data.gete(); 
  _ezero = data.getezero();
  _iat = data.getiat();
  _ia = data.getia();
  _iz = data.getiz();
  _izt = data.getizt();

  _ix = data.getix();
  _iy = data.getiy();
  

  _data = data;

}

ActSTSelect::~ActSTSelect() 
{
  // Destructor
}

std::string ActSTSelect::findFormula() {

  std::string formula("");

  // Check for limiting cases
  if (_izt < 6) {
    cout<<"Program works only for zt > 5"<<endl;
    return formula;
  }

  if (_iz == 1) {
    formula = "trit";
    return formula;
  }

  // (p,n), (n,p) and (p,xn), x <= 4
  if ( (_iy == 0 && _ix <= 4) || (_iy == 2 && _ix == -1) ||
       (_iy == 1 && _ix <= 5 && _izt >= 8) ) {
    formula = "prph";
    return formula;
  }

  // (p,pn) and (p,2p)
  if ((_iat - _ia) == 1) {
    formula = "prph";
    return formula;
  }

  if (_iat >= 35 && _iat <= 209) {

    // (p,pxn)
    int ixmax = (int) ((_at/66.0) + 3.0);

    if (_iat > 70) {
      double term1 = (209.0 - _at)/139.0;
      double xmax = _at*0.05 + 1.5*term1*term1 + 0.8;
      ixmax = (int) (xmax);
    }

    if (_iy == 1 && _ix >= 2 && _ix <= ixmax) {
      formula = "prph";
      return formula;
    }

    // (p,2pxn)

    if (_iy == 2) {
      double exponent = (_at - 39.0)/54.5;
      double expTerm = 1.0 - exp(-exponent*exponent);
      ixmax = (int) (1.0 + 10.0*expTerm);
      ixmax = std::max(ixmax, 2);
      if (_ix <= ixmax) {
	formula = "prph";
	return formula;
      }
    }

    // (p,3pxn)

    if (_iy >= 3 && _iy <= 5) {
      ixmax = 0;
      if (_iat > 70) {ixmax = (int) ((_at/25.0) + 0.501);}
      if (_ix <= ixmax) {
	formula = "prph";
	return formula;
      }
    }
  }

  // Light nuclei (zt < 29)

  if (_izt <= 28 && _iz <= 4 && _iz >= 2 && _ia >= 6 && _ia <= 12) {
    formula = "lit1";
    return formula;
  }

  // Put boron in with lit2-4 spallation (ST'85 p 878)

  if (_izt <= 16 && _izt >= 6 && _iz >= 5) {
    formula = "lit2";
    return formula;
  }

  // izt = 20 and 21 have special geometric mean calculation (lit5, below)
  if (_izt < 20 && _izt >= 17 && _iz >= 5) {
    formula = "lit3";
    return formula;
  }

  if (_izt > 21 && _izt <= 28 && _iz >= 5) {
    formula = "lit4";
    return formula;
  }

  // Special case of Ca and Sc (ST'85, page 878)
  if ((_izt == 20 || _izt == 21) && _iz >= 5) {
    formula = "lit5";
    return formula;
  }

  // Medium nuclei (at <= 68)

  if (_izt >= 29 && _iat <= 68) {

    if (_ia >= 6 && _iz <= 5) {

      // Put boron in with evaporation products rather than spallation
      formula = "evap";
      return formula;
    }

    if (_iz >= 6) {
      formula = "spal";
      return formula;
    }
  }

  // Medium-heavy nuclei (69 <= at <= 109)

  if (_iat <= 109 && _iat >= 69) {

    if (_ia >= 6 && _iz <= 4) {
      formula = "evap";
      return formula;
    }

    // Calculate delta A and A(critical)

    double delacVal = this->delac(_at, _ezero, _ezero);

    if ((_at - _a) > delacVal) {
      formula = "brkp";
    } else {
      formula = "spal";
    }
    return formula;
  }

  // Heavy nuclei (110 <= at <= 209)
  if (_iz <= 4 && _ia >= 6) {
    formula = "evap";
    return formula;
  }

  if (_iz > 4 && _a <= 0.23*_at) {
    formula = "brkp";
    return formula;
  }

  if (0.23*_at < _a && 56 >= _ia) {
    formula = "mxbf";
    return formula;
  }

  // Calculate delta A(critical)

  double dela = _at - _a;

  if (_ia >= 57) {

    double delacVal = this->delac(_at, _ezero, _ezero);
    if ((delacVal - dela) >= 20.0) {
      formula = "spal";
      return formula;
    }

    // To get here we must have delacVal-dela < 20
    if (_iat <= 125) {
      formula = "mxfs";
      return formula;
    }

    // To get here we must in addition have at > 125

    double nzcVal = nzc(_at, _zt, dela, _ezero, _atbar);
    double ratio = 0.0;
    if (fabs(_zt) >= 1e-30) {ratio = (_at - _zt)/_zt;}
    if (ratio <= nzcVal) {
      formula = "spal";
      return formula;
    }

    // To get here we must in addition have n/z > n/zc

    if (_iz > 57) {
      formula = "spal";
    } else if (_iz < 51) {
      formula = "mxfs";
    } else {
      formula = "fgsg";
    }
  } // end of a >= 57 loop

  return formula;

}

double ActSTSelect::delac(double at, double e, double ezero) {

  double delac(0.0);

  if (e < ezero) {
    double term = (log(e) - 3.17)*0.052*(at - 36.0) + 31.5;
    int termInt = (int) (term);
    delac = termInt*1.0;
  } else {
    double term = (log(at) + 1.23)*0.045*(at - 36.0) + 31.5;
    int termInt = (int) (term);
    delac = termInt*1.0;
  }

  return delac;

}

double ActSTSelect::nzc(double at, double zt, double dela, 
			double ezero, double atbar) {

  double delacVal = this->delac(at, ezero, ezero);
  double ratio = 0.0;
  if (fabs(zt) >= 1e-30) {ratio = (at - atbar)/zt;}
  double nzc = 0.005*(delacVal - dela) + 0.3*ratio + 1.29;
  return nzc;

}
