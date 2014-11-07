// A class to keep track of what has been calculated (cross-sections, yields)
// Use this to update GUI progress bars/cout statements etc..

#include "Activia/ActAbsCalcStatus.hh"

ActAbsCalcStatus::ActAbsCalcStatus() : _runCode(true), _nTIsotopes(0), _iTIsotope(0), 
				       _nPIsotopes(0), _iPIsotope(0)
{
  // Constructor
  this->setUp();
}

ActAbsCalcStatus::~ActAbsCalcStatus() 
{
  // Destructor
}

void ActAbsCalcStatus::setNTargetIsotopes(int nIsotopes) {
  _nTIsotopes = nIsotopes;
}

void ActAbsCalcStatus::setTargetIsotope(int iIsotope) {

  if (iIsotope > 0 && iIsotope < _nTIsotopes+1) {
    _iTIsotope = iIsotope;
  }
}

void ActAbsCalcStatus::setNProductIsotopes(int nIsotopes) {
  _nPIsotopes = nIsotopes;
}

void ActAbsCalcStatus::setProductIsotope(int iIsotope) {

  if (iIsotope > 0 && iIsotope < _nPIsotopes+1) {
    _iPIsotope = iIsotope;
  }

}
