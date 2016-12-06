// Read and interpolate the cross-section from tables of sigma vs energy
// for target-product pairs (e.g from MENDL-2 data tables).

#include "Activia/ActXSecDataModel.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActTargetNuclide.hh"
#include "Activia/ActNuclide.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActString.hh"
#include "Activia/ActNuclideFactory.hh"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;

ActXSecDataModel::ActXSecDataModel(std::string name, std::string inputList, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _inputList = inputList;
  _listOfDataFiles.clear();
  _dataTables.clear();

  _theGraph = ActXSecGraph();
  _nullGraph = ActXSecGraph();
  _Zt = 0; _Z = 0;
  _At = 0.0; _A = 0.0;

  this->initialise();
}

ActXSecDataModel::~ActXSecDataModel() 
{
  // Destructor
}

void ActXSecDataModel::initialise() {

  // Store the list of the file names containing the cross-section
  // energy data tables. These tables must have a common format:
  // # Initial comment lines (start line with # symbol)
  // Target Zt At
  // # New table (comment must be written to start a new product table)
  // Product Z A
  // E (MeV)  sigma (mb)

  cout<<"Within ActXSecDataModel::initialise; inputList = "<<_inputList<<endl;
  std::ifstream getData(_inputList.c_str());

  // Check if the file exists
  if (!getData.is_open()) {
    cout<<"Error in ActXSecDataModel::initialise; inputList "<<_inputList
	<<" does not exist. Will not store any data table information."<<endl;
    getData.close();
  }

  while (getData.good()) {
    
    std::string fileName("");
    
    getData >> fileName;
    
    if (getData.eof()) {break;}
    
    if (_debug == 1) {cout<<"Adding the file "<<fileName<<endl;}
    _listOfDataFiles.push_back(fileName);
    
  }

  cout<<"There are "<<_listOfDataFiles.size()<<" data files loaded"<<endl;

}

void ActXSecDataModel::loadDataTables(ActTargetNuclide* targetIsotope,
				      ActProdNuclideList* prodList) {
  
  if (targetIsotope == 0) {
    cout<<"Error in ActXSecDataModel::loadDataTables. Target isotope is null"<<endl;
    return;
  }

  if (prodList == 0) {
    cout<<"Error in ActXSecData::loadDataTables. List of products is null"<<endl;
    return;
  }

  ActNuclide* targetNuclide = targetIsotope->getNuclide();
  // Find out if we have already stored the data table for this target isotope/nuclide
  ActXSecDMAllMap::iterator iter = _dataTables.find(targetNuclide);

  if (iter != _dataTables.end()) {

    if (_debug == 1) {
      cout<<"Data tables already loaded for ";
      targetNuclide->print();
    }
    return;

  }

  // We have to construct the map for the target nuclide
  int nFiles = (int) _listOfDataFiles.size();
  int iFile;
  for (iFile = 0; iFile < nFiles; iFile++) {

    std::string fileName = _listOfDataFiles[iFile];

    this->storeXSecGraphs(fileName, targetNuclide, prodList);

  }

}

void ActXSecDataModel::storeXSecGraphs(std::string fileName, 
				       ActNuclide* targetNuclide,
				       ActProdNuclideList* prodList) {
  
  //cout<<"Within storeXSecGraphs using fileName = "<<fileName<<endl;
  if (targetNuclide == 0 || prodList == 0) {
    cout<<"Error. TargetNuclide and/or prodList are null."<<endl;
    return;
  }

  // Read the input data file. This data file should only be for
  // one target isotope (but can contain many product isotopes).
  std::ifstream getData(fileName.c_str());
  std::string whiteSpace(" ");

  double energy(0.0), sigma(0.0), prodRate(0.0);

  ActXSecGraph* graph(0);
  ActNuclide* prodNuclide(0);
  ActDataModelMap graphMap;

  int targetZ = targetNuclide->getZ();
  double targetA = targetNuclide->getA();
  
  while (getData.good()) {

    // Read various input lines
    if (getData.peek() == '\n') {
      
      // Finish reading line
      char c;
      getData.get(c);

      // Stop while loop if we have reached the end of the file
      if (getData.eof()) {break;}

    } else if (getData.peek() == '#') {

      // Skip comment line
      getData.ignore(1000, '\n');
      getData.putback('\n');
      
      // Stop while loop if we have reached the end of the file
      if (getData.eof()) {break;}

    } else {

      // Read data line
      char line[100];
      getData.getline(line, 100);

      // Stop while loop if we have reached the end of the file
      if (getData.eof()) {break;}

      ActString lineString(line);
      // Split up the line according to white spaces
      std::vector<std::string> lineVect = lineString.split(whiteSpace);
      int nVect = (int) lineVect.size();

      if (nVect < 1) {break;}

      // We should have a line specifying the target or product isotope
      std::string firstStr = lineVect[0];

      if (firstStr.compare("Target") == 0 || firstStr.compare("target") == 0) {

	// Check if we have the same Z and A values
	int Zt = atoi(lineVect[1].c_str());
	double At = atof(lineVect[2].c_str());
	// If we don't have the same values, exit this function, since
	// this file does not have the right data...
	if (!(Zt == targetZ && std::fabs(At - targetA) < 1e-10)) {return;}

      } else if (firstStr.compare("Product") == 0 || firstStr.compare("product") == 0) {

	// We have the product isotope (or a new one)
	// Before processing the new product isotope, store the previous graph
	// into the map
	if (graph != 0 && prodNuclide != 0) {
	  graphMap[prodNuclide] = *graph;	   
	}
	
	int Z = atoi(lineVect[1].c_str());
	double A = atof(lineVect[2].c_str());
	prodNuclide = prodList->getProdNuclide(Z, A);
	if (prodNuclide == 0) {
	  prodNuclide = ActNuclideFactory::getInstance()->getNuclide(Z, A, 0.0);
	}

	// Create a new XSecGraph
	if (graph != 0) {delete graph; graph = 0;}
	graph = new ActXSecGraph();
	
      } else if (graph != 0 && nVect >= 2) {

	// Read the sigma, energy data lines
	energy = atof(firstStr.c_str());
	sigma = atof(lineVect[1].c_str());
	// Store energy and sigma in the graph (prodRate = 0 for now).
	graph->addPoint(energy, sigma, prodRate);

      }
    } // reading data lines

  } // loop over data file lines

  // Store the last product nuclide xSec graph into the map
  if (graph != 0 && prodNuclide != 0) {	    
    graphMap[prodNuclide] = *graph;	   
  }

  // Now assign the target isotop to the xSec graph map
  _dataTables[targetNuclide] = graphMap;

}


ActXSecGraph ActXSecDataModel::getXSecGraph(ActNucleiData* data) {

  // Get the target and product isotopes, then get the graph from the map
  if (data != 0) {

    // First check if we already have the graph extracted in memory by checking if we 
    // have already processed the same target-product pair. This is
    // significantly faster than trying to find the graph using map iterators.
    int Zt = data->getizt();
    int Z = data->getiz();
    double At = data->getat();
    double A = data->geta();

    if (Zt == _Zt && std::fabs(At - _At) < 1e-6 &&
	Z == _Z && std::fabs(A - _A) < 1e-6) {

      // We have the same product and isotope information. Return the
      // already found graph (or the null Graph - the first graph assigned).
      return _theGraph;

    } else {

      // We have not already had this target-product pair.
      // Set the internal A and Z values for the target and product.
      // The rest of the function should then find the map between them,
      // if it exists. If the next call to getXSecGraph has the same target/product
      // information, then return the already found graph or the null graph.
      _Zt = Zt; _At = At; _Z = Z; _A = A;
      // Assign the graph to a null one at first.
      _theGraph = _nullGraph;
      // The remainder of this function will try to find the graph, if it exists.
    }

    // Get the target nuclide pointer from the data
    ActTargetNuclide* targetIsotope = data->getTargetIsotope();

    if (targetIsotope != 0) {

      // Get the nuclide pointer for the target
      ActNuclide* targetNuclide = targetIsotope->getNuclide();

      // See if a <product, graph> map exists for the target Nuclide
      ActXSecDMAllMap::iterator iter = _dataTables.find(targetNuclide);

      if (iter != _dataTables.end()) {

	// Get the product nuclide and the <product, graph> map
	ActNuclide* prodNuclide = data->getProduct();
	ActDataModelMap dataMap = iter->second;

	// Loop over the map to get the graph for the given product
	ActDataModelMap::iterator mapIter = dataMap.find(prodNuclide);

	if (mapIter != dataMap.end()) {

	  // We have found a graph.
	  _theGraph = mapIter->second;

	  if (_debug == 1) {

	    cout<<"Got map for "<<endl;
	    targetNuclide->print(); prodNuclide->print();

	    cout<<"Graph has "<<_theGraph.numberOfPoints()<<" points"<<endl;

	    std::vector<ActGraphPoint> points = _theGraph.getPoints();
	    int iP;
	    int nP = points.size();
	    for (iP = 0; iP < nP; iP++) {
	      ActGraphPoint point = points[iP];
	      cout<<"graph point "<<iP<<", E = "<<point.getX()
		  <<", sig = "<<point.getY()<<endl;

	    } // loop over points

	  } // debug printouts

	} // Get the graph

      } // found targetIsotope in map

    } // targetIsotope != 0

  } // data != 0

  return _theGraph;

}

double ActXSecDataModel::calcCrossSection(ActNucleiData* data) {

  // If the target isotope already exists, just retrieve the stored data.
  // Find out if the energy range is valid, and perform a linear
  // interpolation to get the cross-section

  _sigma = 0.0;
  if (data == 0) {return _sigma;}

  ActXSecGraph xSecGraph = this->getXSecGraph(data);

  double energy = data->gete();
  _sigma = xSecGraph.calcSigma(energy);

  return _sigma;

}

