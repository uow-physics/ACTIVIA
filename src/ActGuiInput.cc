#ifdef ACT_USE_QT

// Abstract class for inputing data
// Classes inherting from this must implement various functions.
#include "Activia/ActGuiInput.hh"
#include "Activia/ActTarget.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActProdNuclide.hh"
#include "Activia/ActTime.hh"
#include "Activia/ActCosmicSpectrum.hh"
#include "Activia/ActSTXSecAlgorithm.hh"
#include "Activia/ActSimpleDecayAlgorithm.hh"

#include "QtGui/qtablewidget.h"

#include <iostream>
using std::cout;
using std::endl;

ActGuiInput::ActGuiInput() : ActAbsInput()
{
  _calcInt = ActAbsInput::AllProducts;
  _theGui = 0;
}

ActGuiInput::ActGuiInput(ActOutputSelection* outputSelection,
			 ActGuiWindow* theGui) : ActAbsInput(outputSelection)
{
  _theGui = theGui;
}

ActGuiInput::~ActGuiInput() 
{
  // Destructor

}

void ActGuiInput::defineTarget() {

  if (_theGui == 0) {return;}

  if (_target != 0) {delete _target;}
  int targetZ = _theGui->getTargetZ();
  _target = new ActTarget(targetZ);

  QTableWidget* targetTable = _theGui->getTargetTable();

  int nIsotopes = targetTable->rowCount();

  int iIso;
  double totalFrac(0.0), halfLife(0.0);
  for (iIso = 0; iIso < nIsotopes; iIso++) {

    QTableWidgetItem* AEntry = targetTable->item(iIso, 0);
    QTableWidgetItem* fEntry = targetTable->item(iIso, 1);
    if (AEntry != 0 && fEntry != 0) {

      double A = AEntry->text().toDouble();
      double fraction = fEntry->text().toDouble();
      totalFrac += fraction;
      _target->addIsotope(A, fraction, halfLife);

    }

  }

  cout<<"Total isotope fraction sum = "<<totalFrac<<endl;

}

void ActGuiInput::defineCalcMode() {

  bool doSingle = _theGui->doSingle();

  if (doSingle == true) {
    _calcInt = ActAbsInput::SingleProd;
  } else {
    _calcInt = ActAbsInput::AllProducts;
  }

  cout<<"Calculation mode is "<<_calcInt<<endl;

}

void ActGuiInput::defineNuclides() {

  if (_prodNuclideList != 0) {delete _prodNuclideList;}
  _prodNuclideList = new ActProdNuclideList();

  std::string isotopeFile = _theGui->getIsotopeFileName();

  if (_calcInt == ActAbsInput::SingleProd) {

    ActProdNuclideList getProdNuclides;
    getProdNuclides.storeTable(isotopeFile.c_str());

    int prodZ = _theGui->getSingleZ();
    double prodA = _theGui->getSingleA();    

    ActProdNuclide* nuclide = getProdNuclides.getProdNuclide(prodZ, prodA);
    if (nuclide != 0) {

      // Store the new prodNuclide pointer into the _prodNuclideList object.
      // When _prodNuclideList is deleted, the new pointer will also be deleted.
      _prodNuclideList->addProdNuclide(*nuclide);

    } else {
      
      // We can not find this isotope in the decay table. Just add the Z,A nuclide
      // to the _prodNuclideList object with no half life nor side branch information.
      ActProdNuclide* nuclide = new ActProdNuclide(prodZ, prodA, 0.0);
      nuclide->print();
      _prodNuclideList->addProdNuclide(*nuclide);

    } 

  } else if (_calcInt == ActAbsInput::AllProducts) {

    _prodNuclideList->storeTable(isotopeFile.c_str());

  }
  
}

void ActGuiInput::defineSpectrum() {

  if (_spectrum != 0) {delete _spectrum;}

  cout<<"Choosing cosmic ray spectrum to specify the input beam."<<endl;
  int ZBeam(1);
  double ABeam(1.0);
  _spectrum = new ActCosmicSpectrum("CosmicRays", ZBeam, ABeam);

  double EStart = _theGui->getEStart();
  double EEnd = _theGui->getEEnd();
  double dE = _theGui->getdE();
  _spectrum->setEnergies(EStart, EEnd, dE);

}

void ActGuiInput::specifyXSecAlgorithm() {

  if (_xSecAlgorithm != 0) {delete _xSecAlgorithm;}

  bool useDataTables = _theGui->useDataTableFile();

  if (useDataTables == false) {

    cout<<"Not using any data tables"<<endl;
    _xSecAlgorithm = new ActSTXSecAlgorithm();
    
  } else {    

    std::string dataTablesFile = _theGui->getDataTablesFile();
    double minDataXSec = _theGui->getMinSigma();    
    cout<<"Using data tables from "<<dataTablesFile<<", minSigma = "<<minDataXSec<<endl;
    
    _xSecAlgorithm = new ActSTXSecAlgorithm(dataTablesFile.c_str(), minDataXSec);
    
  }  

}

void ActGuiInput::defineTime() {

  double texp = _theGui->getExposureTime();
  double tdec = _theGui->getDecayTime();
  if (_time != 0) {delete _time;}
  _time = new ActTime(texp, tdec);

}

void ActGuiInput::specifyDecayAlgorithm() {

  if (_decayAlgorithm != 0) {delete _decayAlgorithm;}

  cout<<"Using simple exponential radioactive decay algorithm."<<endl;  
  _decayAlgorithm = new ActSimpleDecayAlgorithm(_target, _prodNuclideList, _time);

}

void ActGuiInput::specifyOutput() {
  // Nothing to do here, since the GUI sets the output file names, formats and
  // level of detail directly to the outputSelection pointer.
}

#endif
