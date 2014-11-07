// Abstract class for inputing data
// Classes inherting from this must implement various functions.

#include "Activia/ActAbsInput.hh"
#include "Activia/ActTarget.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActTime.hh"
#include "Activia/ActBeamSpectrum.hh"
#include "Activia/ActAbsXSecAlgorithm.hh"
#include "Activia/ActAbsDecayAlgorithm.hh"
#include "Activia/ActOutputSelection.hh"

ActAbsInput::ActAbsInput() : _target(0), _prodNuclideList(0), _spectrum(0),
			     _xSecAlgorithm(0), _time(0), _decayAlgorithm(0),
			     _outputSelection(0), _calcInt(-1), _option("")
{
  // Constructor
}

ActAbsInput::ActAbsInput(ActOutputSelection* outputSelection) : _target(0), _prodNuclideList(0), _spectrum(0),
								_xSecAlgorithm(0), _time(0), _decayAlgorithm(0),
								_outputSelection(outputSelection), _calcInt(-1), 
								_option("")
{
  // Constructor
}

ActAbsInput::~ActAbsInput() 
{
  // Destructor
  if (_target != 0) {delete _target; _target = 0;}
  if (_prodNuclideList != 0) {
    delete _prodNuclideList; _prodNuclideList = 0;
  }
  if (_spectrum != 0) {delete _spectrum; _spectrum = 0;}
  if (_xSecAlgorithm != 0) {delete _xSecAlgorithm; _xSecAlgorithm = 0;}

  if (_time != 0) {delete _time; _time = 0;}
  if (_decayAlgorithm != 0) {delete _decayAlgorithm; _decayAlgorithm = 0;}

}

void ActAbsInput::getData() {

  this->defineTarget();
  this->defineCalcMode();
  this->defineNuclides();  
  this->defineSpectrum();
  this->specifyXSecAlgorithm();

  this->defineTime();
  this->specifyDecayAlgorithm();

  this->specifyOutput();

}
