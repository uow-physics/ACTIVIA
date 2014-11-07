// Class to represent target nuclides; halflife is in days.
// Includes isotope fraction inside the target as well as other
// calculated variables, like threshold energies etc..

#include "Activia/ActTargetNuclide.hh"
#include "Activia/ActNuclideFactory.hh"

ActTargetNuclide::ActTargetNuclide(int Z, double A, double halfLife, 
				   double fraction) : ActNuclide(Z, A, halfLife)
{
  // Constructor
  _nuclide = ActNuclideFactory::getInstance()->getNuclide(Z, A, halfLife);
  _fraction = fraction; _meanA = A; _medianA = A; _EZero = 0.0;
}

ActTargetNuclide::ActTargetNuclide(int Z, double A, double halfLife, double fraction, 
				   double meanA, double medianA, double EZero) : ActNuclide(Z, A, halfLife)
{
  // Constructor
  _nuclide = ActNuclideFactory::getInstance()->getNuclide(Z, A, halfLife);
  _fraction = fraction; _meanA = meanA; _medianA = medianA; _EZero = EZero;
}

ActTargetNuclide::~ActTargetNuclide() 
{
  // Destructor
}
