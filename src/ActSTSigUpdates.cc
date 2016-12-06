// Class to update the cross-section calculations for specific
// target and product nuclei. Some corrections need to be applied
// for several models, hence they are in this separate class that
// all can refer to.

#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTSigUpdates::ActSTSigUpdates()
{
  // Constructor
}

ActSTSigUpdates::~ActSTSigUpdates() 
{
  // Destructor
}

void ActSTSigUpdates::updateSigma(ActNucleiData& data, double& sigma) {

  // Update the cross-section sigma by reference
  // Updated factors
  // 1. Enhanced prod of C12 and O16 from Ne, Na, Mg and Al targets (10<=Zt<=13):
  int izt = data.getizt();
  int iz = data.getiz();
  int ia = data.getia();
  int iat = data.getiat();
  int idnp = data.getidnp();

  if (izt >= 10 && izt <= 13) {
    if ((iz == 6 && ia == 12) || (iz == 8 && ia == 16)) {
      sigma *= 2.0;
    }
  }

  // 2. Suppression of F from Ne to S
  if (izt >= 10 && izt <= 16 && iz == 9) {sigma *= 0.8;}

  // 3. Futher suppression factors
  if (izt >= 9 && izt <= 16) {
    if (iz >= 8 && idnp == -1) {sigma *= 0.7;}
  }

  if (izt >= 6 && izt <= 11) {
    if (idnp == -2) {sigma *= 0.4;}
  }

  // 4. Futher suppression factors
  if (izt >= 17 && izt <= 20 && iz > 10) {
    if (idnp <= -1 || idnp >= 3) {
      sigma *= 0.5;
    }
  }

  // 5. Suppression factor for deltaZ = 1 or 3 for 18<=Z<=20
  if (iz >= 18 && iz <= 20) {
    int in = data.getin(); // integer number of neutrons
    if (iz == in) {
      int idz = izt - iz; // deltaZ
      if (idz == 1 || idz == 3) {sigma *= 0.7;}
    }
  }  

  // 6. Production of Mg, Si, Ar and S from Ca-40:
  if (izt == 20 && iat == 40) {
    if (iz == 12 || iz == 14) {
      sigma *= 2.4;
    } else if (iz == 16 || iz == 18) {
      sigma *= 1.6;
    }
  }

  // 7. Production of Ca, Ar and S from Cr-52 and Fe-56.
  // Production of Z=19-22 from Ni-58.
  if (iz == 16 || iz == 18 || iz == 20) {
    if ((izt == 24 && iat == 52) || (izt == 26 && iat == 56)) {
      sigma *= 1.3;
    }
  }
  if (izt == 28 && iat == 58) {
    if (iz >= 19 && iz <= 22) {sigma *= 1.4;}
  }

  // 8. Production of N-14 and N-15 from Ne-20 and Mg-24
  if (iz == 7 && (ia == 14 || ia == 15)) {
    if ((izt == 10 && iat == 20) || (izt == 12 && iat == 24)) {
      sigma *= 1.5;
    }
  }

  // 15. Reduction of neutron-rich products from O-16 and Ne-20
  if ((izt == 8 || izt == 10) && (iat == 2*izt)) {
    if (idnp > 1 && iz > 4) {sigma *= 0.7;}
  }
      
  // 16. Reduction of n-rich products from Cr to Ni
  if (izt >= 24 && izt <= 28 && iz >= 21 
      && iz <= 23 && idnp >= 6) {sigma *= 0.5;}

  // 17. Energy correction for products with large deltaZ for heavy targets
  if (izt > 30) {
    int idz = izt - iz;
    if (idz >= 5) {
      double e = data.gete();
      double exp1 = std::fabs(e - 1230.0)/150.0;
      double exp2 = std::fabs(idz*1.0 - 12.0)/5.0;
      double F = 0.9*exp(-exp1*exp1)*exp(-exp2*exp2) + 1.0;
      sigma *= F;
    }   
  }

  // Correction from ST'85, p876 for even charge products
  if (iat <= 68 && izt >= 17) {
    if (iz <= 16 && iz%2 == 0 && ia >= iat/2) {sigma *= 1.25;}
  }

}
