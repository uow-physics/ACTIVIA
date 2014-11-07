// Class for defining the target, product nuclide list and input beam spectrum
// to calculation target-product isotope cross-sections
// Also implement calls to calculate yields and isotope decay algorithm(s)

#include "Activia/ActIsotopeProduction.hh"

#include "Activia/ActTarget.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActBeamSpectrum.hh"
#include "Activia/ActAbsInput.hh"
#include "Activia/ActAbsXSecAlgorithm.hh"
#include "Activia/ActNuclideFactory.hh"
#include "Activia/ActTime.hh"
#include "Activia/ActAbsOutput.hh"

#include <iostream>
using std::cout;
using std::endl;

ActIsotopeProduction::ActIsotopeProduction(ActAbsInput* inputData, ActAbsOutput* outputData)
{
  // Constructor
  _inputData = inputData;
  _outputData = outputData;
}

ActIsotopeProduction::~ActIsotopeProduction() 
{
  // Destructor
}

void ActIsotopeProduction::calcCrossSections() {

  if (_inputData == 0 || _outputData == 0) {
    cout<<"Error in ActIsotopeProduction. Input and/or output classes are null."<<endl;
    return;
  }

  cout<<"Running ActIsotopeProduction::calcCrossSections"<<endl;

  ActTarget* target = _inputData->getTarget();
  if (target == 0) {
    cout<<"Error in ActRun::run(). Target is null. Exiting."<<endl;
    return;
  }

  ActBeamSpectrum* spectrum = _inputData->getSpectrum();
  if (spectrum == 0) {
    cout<<"Error in ActRun::run() - spectrum is null. Exiting."<<endl;
    return;
  }

  ActProdNuclideList *prodNuclideList = _inputData->getProdNuclideList();
  if (prodNuclideList == 0) {
    cout<<"Error in ActRun::run(). ActProdNuclideList is null"<<endl;
    return;
  }

  ActAbsXSecAlgorithm *algorithm = _inputData->getXSecAlgorithm();
  if (algorithm == 0) {
    cout<<"Error in ActIsotopeProduction. XSecAlgorithm is null"<<endl;
    return;
  }

  // Calculate the cross-sections
  target->setOutput(_outputData);
  target->calcXSections(prodNuclideList, spectrum, algorithm);

}
