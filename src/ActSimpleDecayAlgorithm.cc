// Class for using simple exponential formulae for isotope decay

#include "Activia/ActSimpleDecayAlgorithm.hh"

#include "Activia/ActTarget.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActProdXSecData.hh"
#include "Activia/ActConstants.hh"
#include "Activia/ActProdNuclide.hh"
#include "Activia/ActNuclide.hh"
#include "Activia/ActTime.hh"
#include "Activia/ActDecayGraph.hh"
#include "Activia/ActGraphPoint.hh"
#include "Activia/ActAbsOutput.hh"
#include "Activia/ActTargetNuclide.hh"
#include "Activia/ActString.hh"
#include "Activia/ActOutputTable.hh"
#include "Activia/ActBeamSpectrum.hh"
#include "Activia/ActAbsCalcStatus.hh"

#include <vector>
#include <cmath>

using std::cout;
using std::endl;

ActSimpleDecayAlgorithm::ActSimpleDecayAlgorithm(ActTarget* target, ActProdNuclideList* prodList,
						 ActTime* times) :
  ActAbsDecayAlgorithm(target, prodList, times)
{
  // Constructor
}

ActSimpleDecayAlgorithm::~ActSimpleDecayAlgorithm() 
{
  // Destructor
}

void ActSimpleDecayAlgorithm::calculateDecays(ActAbsOutput* output) {

  if (_theTarget == 0 || _theProdList == 0 || _times == 0) {return;}
  if (output == 0) {return;}

  _outputData = output;

  _decayMap.clear();

  // Print out some info to screen (as well as to output file)
  this->writeOutputPreamble();

  int nisott = _theTarget->getNIsotopes();
  std::vector<ActTargetNuclide*> targetIsotopes = _theTarget->getIsotopes();

  // Retrieve the xSection data graphs for this target isotope
  std::vector<ActProdXSecData*> xSecDataVector = _theTarget->getXSections();

  // Number of product isotopes in the input table
  int nisotp = _theProdList->getNProdNuclides();

  // Vectors to store tallies of initial and final isotope rates
  std::vector<double> dndti(nisotp), dndtf(nisotp);

  int ip;
  for (ip = 0; ip < nisotp; ip++) {
    dndti[ip] = 0.0; dndtf[ip] = 0.0;
  }

  double texp = _times->getExposureTime();
  double tdec = _times->getDecayTime();
  double totalT = texp+tdec; // sum of exposure and cooling times

  // Create an output table showing the initial decay rate data
  
  ActString words("Target has (Zt,At), product has (Z,A); R = production rate (per kg per day)");
  _outputData->outputLineOfText(words);

  words = ActString("R_tot = Sum over R, including all side branches (SB)");
  _outputData->outputLineOfText(words);

  words = ActString("I0 = Total product yield rate at start of cooling (per kg per day)");
  _outputData->outputLineOfText(words);

  words = ActString("SB = Side branch nucleus (1 = yes, 0 = no). These have R_tot = 0 = I0.");
  _outputData->outputLineOfText(words);
  _outputData->outputLineOfText("");

  std::vector<ActString> columns(8);
  columns[0] = ActString("Zt");
  columns[1] = ActString("At");
  columns[2] = ActString("Z");
  columns[3] = ActString("A");
  columns[4] = ActString("SB");
  columns[5] = ActString("R");
  columns[6] = ActString("R_tot");
  columns[7] = ActString("I0");
  ActOutputTable initialData("initialYields", columns);

  // Calculation status
  ActAbsCalcStatus* calcStatus = _outputData->getCalcStatus();
  bool runCode(true); // calculation status can say "stop calculating"

  // Loop over the target nuclei
  int it;

  for (it = 0; it < nisott; it++) {

    // Set which target isotope is being worked on in the calculation status
    if (calcStatus != 0) {
      calcStatus->setTargetIsotope(it);
      runCode = calcStatus->canRunCode();
    }
    // Stop calculation if requested by the status
    if (runCode == false) {break;}     

    ActNuclide* targetIsotope = _theTarget->getIsotope(it);
    if (targetIsotope == 0) {
      cout<<"Error in target isotope loop. Skipping to next in target"<<endl;
      continue;
    }

    int zt = targetIsotope->getZ();
    double atgt = targetIsotope->getA();

    ActProdXSecData* xSecData = xSecDataVector[it];

    if (xSecData == 0) {
      cout<<"Error. Production xSection data is null"<<endl;
      continue;
    }

    // Get the map storing the xSection data for all product isotopes
    ActProdXSecData::ActProdXSecMap xSecMap = xSecData->getXSecData();

    // Loop over all products. Retrieve the total production rate for activation calculations.
    // Also write out any output about the initial yields etc..
    for (ip = 0; ip < nisotp; ip++) {
      
      // Set which product isotope is being worked on in the calculation status
      if (calcStatus != 0) {calcStatus->setProductIsotope(ip);}

      ActProdNuclide* prodNuclide = _theProdList->getProdNuclide(ip);
      if (prodNuclide == 0) {
	cout<<"Error in product nuclide loop. Nuclide number "<<ip<<" is null."<<endl;
	continue;
      }

      double totSBProdRate(0.0);
      int nSideBranches = prodNuclide->getNSideBranches();
      int iSB;
      for (iSB = 0; iSB < nSideBranches; iSB++) {

	ActNuclide* sideBranch = prodNuclide->getSideBranch(iSB);
	if (sideBranch != 0) {
	  
	  std::vector<double> rowOfData;
	  rowOfData.push_back(zt);
	  rowOfData.push_back(atgt);
	  rowOfData.push_back(sideBranch->getZ());
	  rowOfData.push_back(sideBranch->getA());
	  
	  // Retrieve the XSecGraph (which only has one entry - total production rate/sigma)
	  ActProdXSecData::ActProdXSecMap::iterator iter = xSecMap.find(sideBranch);
	  double SBProdRate(0.0);
	  if (iter != xSecMap.end()) {
	    
	    SBProdRate = iter->second.getTotalProdRate();
	    totSBProdRate += SBProdRate;
	  }

	  rowOfData.push_back(1);
	  rowOfData.push_back(SBProdRate);
	  rowOfData.push_back(0.0);
	  rowOfData.push_back(0.0);

	  initialData.addRow(rowOfData);

	} // Side branch exists
 
      } // Side branch loop

      // Print out activation numbers for the product nuclide
      ActNuclide* product = prodNuclide->getProduct();

      double halfLife = prodNuclide->getHalfLife();
      ActProdXSecData::ActProdXSecMap::iterator iter = xSecMap.find(product);

      // Get the total production rate for this product (which includes side branches)
      double totProdRate(0.0);
      if (iter != xSecMap.end()) {
	totProdRate = iter->second.getTotalProdRate();
      }

      double texpPow(0.0), tdecPow(0.0);
      if (halfLife > 0.0) {
	texpPow = this->calcExpFactor(texp, halfLife);
	tdecPow = this->calcExpFactor(tdec, halfLife);
      }

      // Decay the product nuclide, which is the sum (over all energies)
      // of the side branch nuclides.
      double dndtii(0.0), dndtff(0.0);
      double tCutOff(23.0);

      if (texpPow > tCutOff) {
	dndtii = totProdRate;
      } else {
	dndtii = totProdRate*(1.0 - exp(-texpPow));
      }
            
      dndti[ip] += dndtii;

      if (tdecPow > tCutOff) {
	dndtff = 0.0;
      } else {
	dndtff = dndti[ip]*exp(-tdecPow);
      }
      
      dndtf[ip] = dndtff;

      // Output to file

      std::vector<double> rowOfData;
      rowOfData.push_back(zt);
      rowOfData.push_back(atgt);
      rowOfData.push_back(product->getZ());
      rowOfData.push_back(product->getA());
      rowOfData.push_back(0);
      
      double prodRate = totProdRate - totSBProdRate;

      rowOfData.push_back(prodRate);
      rowOfData.push_back(totProdRate);
      rowOfData.push_back(dndtii);
      
      initialData.addRow(rowOfData);

      // Update calculation status
      if (calcStatus != 0) {calcStatus->decayReport();}

    } // close product isotope loop (ip)

  } // close target isotope loop (nisott)

  // Print initial yields to output file
  
  _outputData->outputTable(initialData);

  // Store the decay rates for the product nuclides in graphs
  for (ip = 0; ip < nisotp; ip++) {

    ActProdNuclide* prodNuclide = _theProdList->getProdNuclide(ip);
    if (prodNuclide != 0) {
      ActNuclide* product = prodNuclide->getProduct();

      ActDecayGraph decayGraph("decayGraph");
      // Add yield rate for total beam exposure time
      decayGraph.addPoint(texp, dndti[ip]);
      // Add final yield rate after cooling time. Note that the time
      // is total time = t_exposure + t_cooling.
      decayGraph.addPoint(totalT, dndtf[ip]);

      // Insert the graph into the decay map
      _decayMap[product] = decayGraph;

    }
  } // Loop over products

  // Print out the summary decay yields/rates

  _outputData->outputLineOfText(""); // empty line for output space
  _outputData->outputLineOfText("Summary Table, summed and weighted by all target isotope fractions");
  _outputData->outputLineOfText("ip: Product list number, tHalf is the (Z,A) product half-life (days)");
  _outputData->outputLineOfText("dndti = yield at start of cooling = sum(I0), dndtf = yield at end of cooling (per kg per day)");

  std::vector<ActString> summaryCols(6);
  summaryCols[0] = ActString("ip");
  summaryCols[1] = ActString("Z");
  summaryCols[2] = ActString("A");
  summaryCols[3] = ActString("tHalf");
  summaryCols[4] = ActString("dndti");
  summaryCols[5] = ActString("dndtf");
  ActOutputTable summaryTable("decaySummary", summaryCols);

  for (ip = 0; ip < nisotp; ip++) {
    
    ActProdNuclide* prodNuclide = _theProdList->getProdNuclide(ip);
    if (prodNuclide != 0) {
      
      ActNuclide* product = prodNuclide->getProduct();

      ActDecayMap::iterator iter = _decayMap.find(product);
      if (iter != _decayMap.end()) {

	ActDecayGraph decayGraph = iter->second;
	std::vector<ActGraphPoint> decayPoints = decayGraph.getPoints();
	int nPoints = (int) decayPoints.size();
	if (nPoints == 2) {

	  double initRate = decayPoints[0].getY();
	  double finalRate = decayPoints[1].getY();

	  std::vector<double> rowOfData;
	  rowOfData.push_back(ip);
	  rowOfData.push_back(product->getZ());
	  rowOfData.push_back(product->getA());
	  rowOfData.push_back(product->getHalfLife());
	  rowOfData.push_back(initRate);
	  rowOfData.push_back(finalRate);

	  summaryTable.addRow(rowOfData);

	} else {
	  cout<<"Problem with decay graph. It has "<<nPoints
	      <<" points when it should only have 2"<<endl;
	}
      }
    } // prodNuclide exists
  } // Loop over prod nuclides

  _outputData->outputTable(summaryTable);

}

double ActSimpleDecayAlgorithm::calcExpFactor(double time, double halfLife) {

  double factor(0.0);
  if (halfLife > 0.0) {factor = time*ActConstants::ln2/halfLife;}
  return factor;

}

void ActSimpleDecayAlgorithm::writeOutputPreamble() {

  if (_theTarget == 0 && _outputData == 0) {return;}
  //if (_outputData->getType() != ActAbsOutput::Stream) {return;}

  int nisott = _theTarget->getNIsotopes();
  std::vector<ActTargetNuclide*> targetIsotopes = _theTarget->getIsotopes();

  _outputData->outputLineOfText("Simple Decay Algorithm");

  ActString words("Target atomic number = ");
  words += _theTarget->getZ();
  words += " with "; words += nisott;
  words += " isotopes";
  _outputData->outputLineOfText(words);

  _outputData->outputLineOfText("Mass numbers and abundances");
  int it;
  for (it = 0; it < nisott; it++) {

    ActTargetNuclide* targetIsotope = _theTarget->getIsotope(it);
    if (targetIsotope != 0) {

      words = ActString("");
      words += targetIsotope->getA();
      words += "   "; words += targetIsotope->getFraction();

      // Get ezero value to print out. This only depends on the target at the moment.
      ActNucleiData data;
      data.setTargetData(targetIsotope);
      data.setEZero();
      words += ", EZero = ";
      words += data.getezero(); words += " MeV";

      _outputData->outputLineOfText(words);

    }

  }
  
  words = ActString("Centroid mass number = ");
  words += _theTarget->getMedianA();
  _outputData->outputLineOfText(words);

  // Print out beam spectrum info (e.g. energy range). 
  ActBeamSpectrum* inputBeam = _theTarget->getInputBeam();
  if (inputBeam != 0) {
    ActString beamWords("Input beam is ");
    beamWords += inputBeam->getName().c_str();
    _outputData->outputLineOfText(beamWords);

    beamWords = ActString("E(start) = ");
    beamWords += inputBeam->getEStart();
    beamWords += " MeV, delta(E) = ";
    beamWords += inputBeam->getdE();
    beamWords += " MeV, n(E) = ";
    beamWords += inputBeam->getnE();
    _outputData->outputLineOfText(beamWords);
  }
  
  if (_times == 0) {return;}
  double texp = _times->getExposureTime();
  double tdec = _times->getDecayTime();

  words = ActString("Exposed for ");
  words += texp; words += " days, cooled down for ";
  words += tdec; words += " days";
  _outputData->outputLineOfText(words);
  _outputData->outputLineOfText("");
    
}
