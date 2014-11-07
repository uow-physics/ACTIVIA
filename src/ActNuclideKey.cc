// Simple class to define a key for an isotope (e.g. Z, A, halfLife)

#include "Activia/ActNuclideKey.hh"

ActNuclideKey::ActNuclideKey(int Z, double A, double halfLife)
{
  // Constructor
  _Z = Z; _A = A; _halfLife = halfLife;
}

ActNuclideKey::~ActNuclideKey() 
{
  // Destructor
}

bool ActNuclideKey::operator < (const ActNuclideKey& other) const {

  bool lessThan(false);

  if (_A < other._A) {return true;}

  if (_A == other._A && _Z < other._Z) {return true;}

  if (_A == other._A && _Z == other._Z && 
      _halfLife < other._halfLife) {return true;}

  return lessThan;

}
