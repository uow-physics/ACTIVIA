// Class to represent nuclides; halflife is in days.

#include "Activia/ActNuclide.hh"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

ActNuclide::ActNuclide(int Z, double A, double halfLife)
{
  // Constructor.
  _Z = Z; _A = A; _fZ = Z*1.0; _halfLife = halfLife;
}

ActNuclide::~ActNuclide() 
{
  // Destructor
}

bool ActNuclide::operator<(const ActNuclide& other) const {

  bool lessThan(false);

  if (_Z < other._Z) {return true;}

  if (_Z == other._Z && _A < other._A) {return true;}

  if (_Z == other._Z && fabs(_A - other._A) < 1e-10 && 
      _halfLife < other._halfLife) {return true;}

  return lessThan;

}

bool ActNuclide::operator==(const ActNuclide& other) const {

  bool equalTo(false);

  if (_Z == other._Z && fabs(_A - other._A) < 1e-10 &&
      fabs(_halfLife - other._halfLife) < 1e-10) {
    equalTo = true;
  }

  return equalTo;

}

void ActNuclide::print() {

  cout<<"Nuclide: Z = "<<_Z<<", A = "<<_A<<" and halfLife = "<<_halfLife<<" days"<<endl;

}
