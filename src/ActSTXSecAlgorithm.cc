// Class for selecting Silberberg-Tsao algorithms for cross-section calculations

#include "Activia/ActSTXSecAlgorithm.hh"

#include "Activia/ActAbsXSecModel.hh"
#include "Activia/ActTargetNuclide.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActNucleiData.hh"

#include "Activia/ActSTSelect.hh"

#include "Activia/ActTritiumModel.hh"
#include "Activia/ActSTSLite.hh"
#include "Activia/ActSTEvaporation.hh"
#include "Activia/ActSTPeripheral.hh"
#include "Activia/ActSTSpallation.hh"
#include "Activia/ActSTBreakup.hh"
#include "Activia/ActSTFission.hh"
#include "Activia/ActSTFissSpallation.hh"
#include "Activia/ActSTFissBreakup.hh"
#include "Activia/ActSTFissSpallGamma.hh"
#include "Activia/ActXSecDataModel.hh"

ActSTXSecAlgorithm::ActSTXSecAlgorithm(const char* listOfDataTables, double minDataXSec) : 
  ActAbsXSecAlgorithm()
{
  // Constructor
  _nucleiData = 0;
  _listOfDataTables = std::string(listOfDataTables);
  this->createListOfModels();
  _currentModel = 0;
  _minDataSigma = minDataXSec;
}

ActSTXSecAlgorithm::~ActSTXSecAlgorithm() 
{
  // Destructor
  delete _trit;
  delete _lit1;
  delete _lit2;
  delete _lit3;
  delete _lit4;
  delete _evap;
  delete _prph;
  delete _spal;
  delete _brkp;
  delete _fiss;
  delete _mxfs;
  delete _mxbf; 
  delete _fgsg;
  delete _dataModel;
}

void ActSTXSecAlgorithm::createListOfModels() {

  int debug(0);

  bool applyUpdates(true);

  // Use a separate tritium model which is not part of Silberberg-Tsao formulae
  _trit = new ActTritiumModel("trit", debug);

  _lit1 = new ActSTSLite("lit1", applyUpdates, debug);
  _lit2 = new ActSTSLite("lit2", applyUpdates, debug);
  _lit3 = new ActSTSLite("lit3", applyUpdates, debug);
  _lit4 = new ActSTSLite("lit4", applyUpdates, debug);
  _lit5 = new ActSTSLite("lit5", applyUpdates, debug);
  _evap = new ActSTEvaporation("evap", applyUpdates, debug);
  _prph = new ActSTPeripheral("prph", applyUpdates, debug);
  _spal = new ActSTSpallation("spal", applyUpdates, debug);
  _brkp = new ActSTBreakup("brkp", applyUpdates, debug);
  _fiss = new ActSTFission("fiss", applyUpdates, debug);
  _mxfs = new ActSTFissSpallation("mxfs", applyUpdates, debug);
  _mxbf = new ActSTFissBreakup("mxbf", applyUpdates, debug);
  _fgsg = new ActSTFissSpallGamma("fgsg", applyUpdates, debug);
  _dataModel = 0;
  if (_listOfDataTables.size() > 0) {
    _dataModel = new ActXSecDataModel("dataModel", _listOfDataTables, debug);
  }

}

void ActSTXSecAlgorithm::setNucleiData(ActNucleiData* data) {

  if (data != 0) {

    // Set the nuclei data object
    _nucleiData = data;

    // Select the cross-section model (which does not depend on energy)
    this->selectXSecModel(data);

  }

}

void ActSTXSecAlgorithm::loadDataTables(ActTargetNuclide* targetNuclide, 
					ActProdNuclideList* prodList) {

  if (_dataModel != 0) {_dataModel->loadDataTables(targetNuclide, prodList);}

}

bool ActSTXSecAlgorithm::passSelection(ActNucleiData* data) {

  // Skip product nuclide if Z or A is too high. 
  // Ensure that we have at least two end products:
  // X + beam -> Y_1 + Y_2 type reactions, not X + beam -> Y + gamma.
  double ztgt = data->getzt();
  double zBeam = data->getzbeam();
  double zCut = ztgt + zBeam;

  double atgt = data->getat();
  double aBeam = data->getabeam();
  double aCut = atgt + aBeam - 1.0;

  bool passed(true);

  double z = data->getz();
  double a = data->geta();
  if (z > zCut || a > aCut) {passed = false;}

  return passed;

}

bool ActSTXSecAlgorithm::passESelection(ActNucleiData* data) {

  bool passed(false);

  if (data != 0) {

    // Get the threshold energy
    double thresholdE = data->getThresholdEnergy();
    // Get the energy
    double energy = data->gete();

    if (energy > thresholdE) {passed = true;}

  }

  return passed;

}

double ActSTXSecAlgorithm::calcCrossSection() {

  double sigma(0.0);

  if (_nucleiData != 0) {

    // First check if we have a non-zero cross-section from
    // data-based tables
    bool gotDataValue(false);

    if (_dataModel != 0) {

      sigma = _dataModel->calcCrossSection(_nucleiData);
      if (sigma > _minDataSigma) {gotDataValue = true;}

    }

    // If we have no sigma value (or one very low), 
    // calculate one based on the Silberberg-Tsao formulae
    if (gotDataValue == false && _currentModel != 0) {
      
      sigma = _currentModel->calcCrossSection(_nucleiData);

    }

  }

  return sigma;

}

void ActSTXSecAlgorithm::selectXSecModel(ActNucleiData* data) {

  _currentModel = 0;

  if (data == 0) {return;}

  ActSTSelect select(*data);

  std::string formula = select.findFormula();

  if (formula == "trit") {

    _currentModel = _trit;

  } else if (formula == "lit1") {
    
    _currentModel = _lit1;

  } else if (formula == "lit2") {

    _currentModel = _lit2;

  } else if (formula == "lit3") {

    _currentModel = _lit3;

  } else if (formula == "lit4") {

    _currentModel = _lit4;
        
  } else if (formula == "lit5") {

    _currentModel = _lit5;

  } else if (formula == "evap") {
    
    _currentModel = _evap;
    
  } else if (formula == "prph") {
    
    _currentModel = _prph;
    
  } else if (formula == "spal") {
    
    _currentModel = _spal;
    
  } else if (formula == "brkp") {
    
    _currentModel = _brkp;
    
  } else if (formula == "fiss") {
 
    _currentModel = _fiss;

  } else if (formula == "mxfs") {
    
    _currentModel = _mxfs;
    
  } else if (formula == "mxbf") {
    
    _currentModel = _mxbf;
    
  } else if (formula == "fgsg") {
    
    _currentModel = _fgsg;
  }  

}
