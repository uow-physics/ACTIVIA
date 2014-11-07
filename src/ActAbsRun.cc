// Class to run the Cosmic code

#include "Activia/ActAbsRun.hh"
 
#include "Activia/ActAbsInput.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActTarget.hh"
#include "Activia/ActTime.hh"

#include "Activia/ActIsotopeProduction.hh"
#include "Activia/ActSimpleDecayAlgorithm.hh"

#include "Activia/ActOutputSelection.hh"
#include "Activia/ActStreamOutput.hh"
#include "Activia/ActROOTOutput.hh"
#include "Activia/ActAbsCalcStatus.hh"

#include <string>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;
using std::cin;

ActAbsRun::ActAbsRun() {
  // Constructor
  _input = 0;
  _outputSelection = new ActOutputSelection();
  _calcStatus = 0;
}

ActAbsRun::~ActAbsRun() {
  // Destructor
  delete _input; _input = 0;
  delete _outputSelection; _outputSelection = 0;
  delete _calcStatus; _calcStatus = 0;
}

void ActAbsRun::run() {

  this->defineInput();
  this->doCalculations();

}

void ActAbsRun::doCalculations() {

  if (_input == 0) {return;}

  _input->getData();

  // Calculate the cross-sections for target-nuclide pairs.

  ActTarget* target = _input->getTarget();
  if (target == 0) {
    cout<<"Error in ActAbsRun::run(). Target is null. Exiting."<<endl;
    return;
  }

  ActTime* times = _input->getTime();
  if (times == 0) {
    cout<<"Error in ActAbsRun::run(). Time is null. Exiting."<<endl;
    return;
  }

  ActProdNuclideList *prodNuclideList = _input->getProdNuclideList();
  if (prodNuclideList == 0) {
    cout<<"Error in ActAbsRun::run(). ActProdNuclideList is null"<<endl;
    return;
  }

  // Calculate the cross-section and production rate (based on the input beam spectrum)
  // for all target-product isotope pairs.

  ActAbsOutput* prodOutput = this->selectXSecOutput();
  if (prodOutput != 0) {
    prodOutput->openFile();
    if (_calcStatus != 0) {prodOutput->setCalcStatus(_calcStatus);}
  }

  ActIsotopeProduction production(_input, prodOutput);
  production.calcCrossSections();
  if (prodOutput != 0) {prodOutput->closeFile();}

  ActAbsOutput* decayOutput = this->selectDecayOutput();
  if (decayOutput != 0) {
    decayOutput->openFile();
    if (_calcStatus != 0) {decayOutput->setCalcStatus(_calcStatus);}
  }

  // Calculate the decay yields from the production rate.
  ActAbsDecayAlgorithm *decayAlgorithm = _input->getDecayAlgorithm();
  if (decayAlgorithm == 0) {
    cout<<"Error in ActAbsRun::run(). ActAbsDecayAlgorithm is null"<<endl;
    return;
  }

  decayAlgorithm->calculateDecays(decayOutput);
  if (decayOutput != 0) {decayOutput->closeFile();}

  // Finalise calculation status
  if (_calcStatus != 0) {_calcStatus->finalise();}

  delete prodOutput;
  delete decayOutput;

}

ActAbsOutput* ActAbsRun::selectXSecOutput() {

  ActAbsOutput* output(0);

  if (_outputSelection != 0) {

    std::string xSecFileName = _outputSelection->getXSecFileName();
    int xSecType = _outputSelection->getXSecType();
    int xSecDetail = _outputSelection->getXSecDetail();

    if (xSecType == ActOutputSelection::ROOT) {
#ifdef ACT_USE_ROOT
      output = new ActROOTOutput(xSecFileName.c_str(), xSecDetail);
#endif
    } else {
      output = new ActStreamOutput(xSecFileName.c_str(), xSecDetail);
    }

    cout<<"Creating output class "<<output->getTypeName()
	<<"; fileName = "<<xSecFileName<<", levelOfDetail flag = "
	<<xSecDetail<<endl;

  }

  // Check if we also want to specify a status check pointer.
  

  return output;

}

ActAbsOutput* ActAbsRun::selectDecayOutput() {

  ActAbsOutput* output(0);

  if (_outputSelection != 0) {

    std::string decayFileName = _outputSelection->getDecayFileName();
    int decayType = _outputSelection->getDecayType();
    int decayDetail = _outputSelection->getDecayDetail();

    if (decayType == ActOutputSelection::ROOT) {
#ifdef ACT_USE_ROOT
      output = new ActROOTOutput(decayFileName.c_str(), decayDetail);
#endif
    } else {
      output = new ActStreamOutput(decayFileName.c_str(), decayDetail);
    }

    cout<<"Creating output class "<<output->getTypeName()
	<<"; fileName = "<<decayFileName<<", levelOfDetail flag = "
	<<decayDetail<<endl;

  }

  return output;

}
