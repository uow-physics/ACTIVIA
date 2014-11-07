// Class to define the list of isotopes for a specific target element (Z value).
// Use this to define the composition of the target volume, not the radioactive
// product isotopes (ActProdNuclides) that can be produced when the input beam hits the 
// target, which are defined in ActProdNuclideList.

#include "Activia/ActTarget.hh"
#include "Activia/ActTargetNuclide.hh"
#include "Activia/ActBeamSpectrum.hh"
#include "Activia/ActNuclideFactory.hh"
#include "Activia/ActProdXSecData.hh"
#include "Activia/ActNuclide.hh"
#include "Activia/ActAbsOutput.hh"
#include "Activia/ActOutputTable.hh"
#include "Activia/ActString.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActProdNuclide.hh"
#include "Activia/ActXSecGraph.hh"

#include <iostream>
#include <cmath>

using std::cout;
using std::endl;

ActTarget::ActTarget(int Z)
{
  // Constructor
  _Z = Z; _fZ = Z*1.0; 

  _isotopes.clear(); _nIsotopes = 0;

  _meanA = 0.0; _medianA = 0.0;
  _xSections.clear();

  _output = 0; _inputBeam = 0;

}

ActTarget::~ActTarget() 
{
  // Destructor
  _isotopes.clear();
  this->clearProdXSecData();
}

void ActTarget::clearProdXSecData() {

  int i;
  int nXSecData = (int) _xSections.size();
  for (i = 0; i < nXSecData; i++) {
    ActProdXSecData* xSecData = _xSections[i];
    delete xSecData; xSecData = 0;
  }
  _xSections.clear();

}

void ActTarget::addIsotope(double A, double fraction, double halfLife) {

  ActTargetNuclide* isotope = new ActTargetNuclide(_Z, A, halfLife, fraction);
  _isotopes.push_back(isotope);

  cout<<"Added isotope "<<A<<" with halfLife = "<<halfLife<<" and abundance fraction "
      <<fraction<<" to element "<<_Z<<endl;
  // Update number of isotopes
  _nIsotopes = (int) _isotopes.size();

  // Recalculate the median and mean values of A
  // as well as the value of E0
  this->calcMedianA();
  this->calcMeanA();

  this->updateIsotopes();

}

void ActTarget::updateIsotopes() {

  // After recalculating medianA and meanA values, update all
  // of the target isotopes with this information.
  int i;
  for (i = 0; i < _nIsotopes; i++) {

    ActTargetNuclide* isotope = _isotopes[i];

    if (isotope != 0) {
      isotope->setMeanA(_meanA);
      isotope->setMedianA(_medianA);
    }
  }
}

void ActTarget::calcXSections(ActProdNuclideList* prodList, ActBeamSpectrum* inputBeam,
			      ActAbsXSecAlgorithm* algorithm) {

  this->clearProdXSecData();

  _inputBeam = inputBeam;

  // Write info to output file (if it exists)
  ActAbsCalcStatus* calcStatus = 0;
  if (_output != 0) {
    _output->outputLineOfText("Cross-sections for target-product nuclide pairs");
    calcStatus = _output->getCalcStatus();
    if (calcStatus != 0) {calcStatus->setNTargetIsotopes(_nIsotopes);}
  }

  int it;
  for (it = 0; it < _nIsotopes; it++) {

    // Update calculation status
    if (calcStatus != 0) {
      calcStatus->setTargetIsotope(it);
    }

    cout<<"Storing xSection results for target isotope "<<it<<endl;
    ActTargetNuclide* targetIsotope = _isotopes[it];

    if (targetIsotope != 0) {

      ActProdXSecData* xSecData = new ActProdXSecData(targetIsotope, prodList, 
						      inputBeam, algorithm, _output);
      xSecData->calculate();

      _xSections.push_back(xSecData);
      
    } else {
      cout<<"Error in ActTarget::calcXSections. Target isotope number "
	  << it <<" is null."<<endl;
    }
  } // Loop over target isotopes

  // Now print out the cross-section data for all product nuclei
  // knowing the relative weights of the target isotopes
  this->outputXSecSummary(prodList);
  
}

void ActTarget::outputXSecSummary(ActProdNuclideList* prodList) {
  
  // Output a summary table of the cross-sections of all of the products
  // by summing over the target isotopes, weighted by the abundance fractions.
  // This is called by calcXSections if the output object is set.
  if (prodList == 0 || _output == 0) {return;}

  ActString words("Summary of product xSections summed over the relative target Z = ");
  words += _Z; words += " isotopes:";
  _output->outputLineOfText(words);
 
  int it;
  for (it = 0; it < _nIsotopes; it++) {

    ActTargetNuclide* targetIsotope = _isotopes[it];
    if (targetIsotope != 0) {

      words = ActString("A = ");
      words += targetIsotope->getA();
      words += ", Fraction = "; words += targetIsotope->getFraction();
      _output->outputLineOfText(words);
    }
  }
  
  std::vector<ActString> columns(4);
  columns[0] = ActString("ProdZ");
  columns[1] = ActString("ProdA");
  columns[2] = ActString("TotSigma");
  columns[3] = ActString("TotProdRate");
  ActOutputTable xSecSummary("xSecSummary", columns);

  int nProducts = prodList->getNProdNuclides();
  int ip;

  for (ip = 0; ip < nProducts; ip++) {
    
    double totSigma(0.0), totProdRate(0.0);

    ActProdNuclide* product = prodList->getProdNuclide(ip);
    if (product == 0) {continue;} // Skip entry if it is null

    // Loop over the target isotopes and calculate the total sigma for this product
    for (it = 0; it < _nIsotopes; it++) {
      
      ActProdXSecData* xSecData = _xSections[it];
      
      // This should be non-zero, but checking in case...
      if (xSecData != 0) {

	ActProdXSecData::ActProdXSecMap xSecMap = xSecData->getXSecData();
	ActProdXSecData::ActProdXSecMap::iterator iter = xSecMap.find(product);

	if (iter != xSecMap.end()) {
	  double fraction = this->getFraction(it);
	  double sigma = iter->second.getTotalSigma();
	  double prodRate = iter->second.getTotalProdRate();
	  totSigma += sigma*fraction;
	  totProdRate += prodRate; // production rate includes abundance fraction
	}

      } // Valid xSec data pointer
      
    } // Loop over target isotopes
    
    std::vector<double> row(4);
    row[0] = product->getfZ();
    row[1] = product->getA();
    row[2] = totSigma;
    row[3] = totProdRate;

    xSecSummary.addRow(row);

  } // Loop over all product nuclei
  
  if (_inputBeam != 0) {
    ActString beamWords("Input beam is ");
    beamWords += _inputBeam->getName().c_str();
    _output->outputLineOfText(beamWords);

    beamWords = ActString("E(start) = ");
    beamWords += _inputBeam->getEStart();
    beamWords += " MeV, delta(E) = ";
    beamWords += _inputBeam->getdE();
    beamWords += " MeV, n(E) = ";
    beamWords += _inputBeam->getnE();
    _output->outputLineOfText(beamWords);
  }

  _output->outputTable(xSecSummary);

}

double ActTarget::getAtomicMass(int index) {

  // Get the atomic mass for the given isotope index
  double A(0.0);

  if (index < 0 || index >= _nIsotopes) {
    cout<<"Error in ActTarget::getAtomicMass. "
	<<"Index is out of bounds [0,"<<_nIsotopes-1<<"]"<<endl;
  } else {
    ActNuclide* isotope = _isotopes[index];
    if (isotope != 0) {A = isotope->getA();}
  }
  
  return A;

}

double ActTarget::getFraction(int index) {

  // Get the abundance fraction for the given isotope index
  double fraction(0.0);

  if (index < 0 || index >= _nIsotopes) {
    cout<<"Error in ActTarget::getFraction. "
	<<"Index is out of bounds [0,"<<_nIsotopes-1<<"]"<<endl;
  } else {
    ActTargetNuclide* isotope = _isotopes[index];
    if (isotope != 0) {fraction = isotope->getFraction();}
  }
  
  return fraction;

}

double ActTarget::getFraction(double A) {

  // Get the abundance fraction for the given atomic mass value
  double fraction(0.0);
  int i;
  for (i = 0; i < _nIsotopes; i++) {

    double AVal(0.0);
    ActTargetNuclide* isotope = _isotopes[i];
    if (isotope != 0) {

      AVal = isotope->getA();

      if (fabs(AVal - A) < 1e-6) {
	fraction = isotope->getFraction();
	break;
      }
    }
  }
    
  return fraction;

}

ActTargetNuclide* ActTarget::getIsotope(int index) {

  if (index < 0 || index >= _nIsotopes) {
    cout<<"Error in ActTarget::getIsotope. Index = "
	<<index<<" is out of range [0,"<<_nIsotopes-1<<"]."<<endl;
    return 0;
  } 

  ActTargetNuclide* isotope = _isotopes[index];

  return isotope;

}

void ActTarget::calcMedianA() {

  _medianA = 0.0;

  int i;
  for (i = 0; i < _nIsotopes; i++) {
    ActNuclide* isotope = _isotopes[i];
    if (isotope != 0) {_medianA += isotope->getA();}
  }

  if (_nIsotopes > 0) {_medianA /= (_nIsotopes*1.0);}

}

void ActTarget::calcMeanA() {

  _meanA = 0.0;

  int i;
  for (i = 0; i < _nIsotopes; i++) {

    ActTargetNuclide* isotope = _isotopes[i];

    if (isotope != 0) {
      _meanA += isotope->getA()*isotope->getFraction();
    }

  }

}
