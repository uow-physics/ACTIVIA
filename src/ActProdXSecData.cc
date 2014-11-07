// Class for defining the production cross-section data for
// a specific target isotope. It stores a map of ActXSecGraphs
// for the target isotope.

#include "Activia/ActProdXSecData.hh"
#include "Activia/ActBeamSpectrum.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActProdNuclide.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActAbsXSecAlgorithm.hh"
#include "Activia/ActConstants.hh"
#include "Activia/ActNuclideFactory.hh"
#include "Activia/ActGraphPoint.hh"
#include "Activia/ActAbsOutput.hh"
#include "Activia/ActOutputSelection.hh"
#include "Activia/ActTargetNuclide.hh"
#include "Activia/ActAbsCalcStatus.hh"

#include <vector>

#include <iostream>
using std::cout;
using std::endl;

ActProdXSecData::ActProdXSecData(ActTargetNuclide* targetIsotope,
				 ActProdNuclideList* prodList,
				 ActBeamSpectrum* inputBeam,
				 ActAbsXSecAlgorithm* algorithm,
				 ActAbsOutput* output)
{
  // Constructor
  _targetIsotope = targetIsotope;
  _prodList = prodList;
  _inputBeam = inputBeam;
  _algorithm = algorithm;
  _xSecData.clear();
  _output = output;
}

ActProdXSecData::~ActProdXSecData() 
{
  // Destructor
  _xSecData.clear();
}

void ActProdXSecData::calculate() {

  // Calculate the target isotope - product cross-sections given the input beam
  // and calculation algorithm

  // Check for null pointers
  if (_prodList == 0 || _targetIsotope == 0) {return;}
  if (_inputBeam == 0 || _algorithm == 0) {return;}

  _xSecData.clear();

  double EStart = _inputBeam->getEStart();
  double dE = _inputBeam->getdE();
  int nE = _inputBeam->getnE();
  cout<<"E0 = "<<EStart<<", dE = "<<dE<<", nE = "<<nE<<endl;
  int nE1 = nE - 1;

  int nProducts = _prodList->getNProdNuclides();
  int ip;

  // Create a nuclei data pointer that will store the beam, target and
  // product information, all in one object, that can be passed around 
  // the various cross-section algorithm/models.
  ActNucleiData* data = new ActNucleiData();
  ActNuclide* beamNuclide = _inputBeam->getNuclide();
  data->setBeamData(beamNuclide);
  data->setTargetData(_targetIsotope);

  // Initialise the production rate normalisation factor
  double factor(0.0);
  double atgt = data->getat();
  if (atgt > 1e-30) {factor = ActConstants::pps/atgt;}
  // Retrieve the abundance fraction for the selected target isotope.
  // This will be used to scale the production rate for all product isotopes.
  double fraction = data->getFraction();

  // Load in any data tables for this target isotope
  // Use the list of product isotopes to store maps of
  // target-product tables.
  _algorithm->loadDataTables(_targetIsotope, _prodList);

  // Store energies in vector (same for all products)
  std::vector<double> energies;
  int iE;
  for (iE = 0; iE < nE; iE++) {
    double energy = iE*dE + EStart;
    energies.push_back(energy);
  }

  int levelOfDetail(0);
  bool outputEGraphs = false;
  if (_output != 0) {
    levelOfDetail = _output->getLevelOfDetail();
    if (levelOfDetail > ActOutputSelection::Summary) {outputEGraphs = true;}
  }

  // Calculation status
  ActAbsCalcStatus* calcStatus = _output->getCalcStatus();
  if (calcStatus != 0) {calcStatus->setNProductIsotopes(nProducts);}
  bool runCode(true); // calculation status can say "stop calculating"

  // Loop over products
  for (ip = 0; ip < nProducts; ip++) {

    cout<<"ActProdXSecData ip = "<<ip<<endl;

    // Set calculation status (if it exists)
    if (calcStatus != 0) {
      calcStatus->setProductIsotope(ip);
      calcStatus->xSecReport();
      runCode = calcStatus->canRunCode();
    }
    // Stop calculation if requested by the status
    if (runCode == false) {break;}

    ActProdNuclide* prodNuclide = _prodList->getProdNuclide(ip);
    
    double zNucl(0.0), aNucl(0.0), halfLife(0.0);
    int nSideBranches(0);
    if (prodNuclide != 0) {
      zNucl = prodNuclide->getfZ();
      aNucl = prodNuclide->getA();
      nSideBranches = prodNuclide->getNSideBranches();
      halfLife = prodNuclide->getHalfLife();
    }

    // Set the product data and check if we pass the selection criteria
    // for the beam, target and product set-up.
    data->setProductData(prodNuclide);
    if (_algorithm->passSelection(data) == false) {
      // We have failed the selection. 
      // Continue to the next product within the (ip) loop.
      continue;
    }
    
    // Define the vector of graph points for the product nuclide.
    std::vector<ActGraphPoint> prodGraphPoints(nE);
    for (iE = 0; iE < nE; iE++) {
      prodGraphPoints[iE] = ActGraphPoint(energies[iE], 0.0, 0.0);
    }

    // Total sigma and production rate for the product (including side branches)
    double totalProdSigma(0.0), totalProdRate(0.0);

    // Loop over side branches, as well as the product isotope
    int iSB;
    for (iSB = 0; iSB < nSideBranches+1; iSB++) {

      // Total sigma and production rate for the individual side branches
      double totalSBSigma(0.0), totalSBProdRate(0.0);

      bool doCalc(true);

      bool sideBranch(false);
      double z(zNucl), a(aNucl), tHalf(halfLife);

      if (iSB != nSideBranches) {

	// We have a side-branch.
	ActNuclide* sbNuclide = prodNuclide->getSideBranch(iSB);

	if (sbNuclide != 0) {
	  z = sbNuclide->getfZ();
	  a = sbNuclide->getA();
	  tHalf = 0.0;
	  sideBranch = true;

	  // Reset the product data to use the side-branch isotope.
	  data->setProductData(sbNuclide);
	  // Check whether the side-branch, together with the beam and target data,
	  // passes the selection criteria. Set the doCalc flag using this check.
	  doCalc = _algorithm->passSelection(data);

	} else {
	  // sbNuclide pointer is null. Skip to the next in the loop.
	  doCalc = false;
	}

      } else if (nSideBranches > 0) {

	// Reset the product data to the original product isotope
	data->setProductData(prodNuclide);

      }

      // Skip nuclide if earlier selections have failed
      if (doCalc == false) {continue;}

      // Also calculate the threshold energy and other quantities, since we now 
      // have the target and product parameters
      data->setOtherQuantities();

      // Set the nuclei data that the algorithm will use
      // Since the data is a pointer, this can be updated and the algorithm
      // will automatically know about the changes.
      _algorithm->setNucleiData(data);

      // Loop over the energy range, calculating the cross section value
      // and storing them in the graph.

      for (iE = 0; iE < nE; iE++) {

	double pfac(1.0);
	if (iE == 0 || iE == nE1) {pfac = 0.5;}

	double energy = energies[iE];
	data->setEnergy(energy);

	bool passESelection = _algorithm->passESelection(data);

	if (passESelection == true) {

	  // Calculate the cross-section based on the previously 
	  // given nuclei data pointer
	  double sigma = _algorithm->calcCrossSection();

	  double dNdE = _inputBeam->fluxdE(energy);
	  double prodRate = pfac*sigma*factor*fraction*dNdE*dE;

	  totalSBSigma += sigma; totalSBProdRate += prodRate;
	  totalProdSigma += sigma; totalProdRate += prodRate;

	  // Also store the sigma and production rate vs energy 
	  // graph for the side branch/product isotope
	  prodGraphPoints[iE].addYValues(sigma, prodRate);

	} // energy selection

      } // energy loop
      
      // Finished looping over all side branches. Store the side-branch summed 
      // sigma/production rate vs energy for the product nuclide using graphs.
      ActNuclide* isotope = ActNuclideFactory::getInstance()->getNuclide((int) z, a, tHalf);

      // Only store the total sigma/production rate to save on RAM memory use!
      ActXSecGraph xSecGraph("xSecGraph");
      if (sideBranch == true) {

	// Only store the total sigma/production rate
	xSecGraph.addPoint(0.0, totalSBSigma, totalSBProdRate);

      } else {

	// Store total sigma/production rate
	xSecGraph.addPoint(0.0, totalProdSigma, totalProdRate);

	// Print out sigma/production rate as function of energy to output class
	ActXSecGraph xSecEGraph("xSecEData");
	xSecEGraph.addPoints(prodGraphPoints);

	if (outputEGraphs == true) {
	  _output->outputGraph(*data, xSecEGraph);
	}

      }
      
      // Insert the graphs into the internal map for the given product nuclide
      //cout<<"Inserting cross-section and production rate vs energy graph into map"<<endl;
      _xSecData[isotope] = xSecGraph;

    } // side branch plus product loop

  } // product loop

  cout<<"Finished in ActProdXSecData"<<endl;

  delete data;

}

