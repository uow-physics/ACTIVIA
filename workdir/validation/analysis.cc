#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TPad.h"
#include "TLegend.h"
#include "TText.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <cstdlib>

using std::cout;
using std::endl;

#include "analysis.hh"

analysis::analysis(int yLog, double yMin, double yMax, int iFlag, int iLabel) {
  _yLog = yLog;
  _yMin = yMin; _yMax = yMax;
  _iFlag = iFlag;
  _iLabel = iLabel;
  _targetName = ""; 
  _dataFileNames.clear(); _authorNames.clear();
  _nDataFiles = 0;
  _sigmaEGraphs.clear();
  _calcNames.clear(); _dataNames.clear();
  this->storeElements("listOfElements.dat");
  this->selectMarkers();
}

analysis::~analysis() {
  _dataFileNames.clear(); _authorNames.clear();
  _sigmaEGraphs.clear();
  _calcNames.clear(); _dataNames.clear();
  _elements.clear();
  _markerStyles.clear();
  _markerColours.clear();
}

void analysis::run(const char* controlFileName) {

  std::ifstream getData(controlFileName);

  getData >> _targetName;

  cout<<"Target name = "<<_targetName<<endl;

  TString inFileName, ntupleName;
  getData >> inFileName >> ntupleName;

  Int_t nDataFiles;
  getData >> nDataFiles;
  Int_t i;
  _dataFileNames.clear();

  cout<<"There are "<<nDataFiles<<" data files"<<endl;

  for (i = 0; i < nDataFiles; i++) {
    TString dataFileName;
    getData >> dataFileName;
    cout<<"Adding data filename "<<dataFileName<<endl;
    _dataFileNames.push_back(dataFileName);
  }

  _nDataFiles = (Int_t) _dataFileNames.size();

  getData >> _dataRootFile;

  TString plotFileName;
  getData >> plotFileName;

  this->storeData(_dataRootFile.Data());

  this->getCalcGraphs(inFileName.Data(), ntupleName.Data());

  // Plot the Activia code output with the data measurement points
  this->plotGraphs(plotFileName.Data());
}

void analysis::plotGraphs(const char* plotFileName) {

  // Get graphs from calculations; overlay measurements from storeData().

  // Retrieve data measurements
  TFile* dataFile = new TFile(_dataRootFile.Data(), "read");
  TTree* dataTree = (TTree*) dataFile->Get("Data");
  Int_t nExpt = (Int_t) dataTree->GetEntries();
  Int_t iExpt, nPoints, iP;

  Double_t dataZ, dataA;
  Double_t dataE[1000], dataEErr[1000];
  Double_t dataSigma[1000], dataSigErr[1000];
  dataTree->SetBranchAddress("Z", &dataZ);
  dataTree->SetBranchAddress("A", &dataA);
  dataTree->SetBranchAddress("nPoints", &nPoints);
  dataTree->SetBranchAddress("E", dataE);
  dataTree->SetBranchAddress("EErr", dataEErr);
  dataTree->SetBranchAddress("sigma", dataSigma);
  dataTree->SetBranchAddress("sigmaErr", dataSigErr);

  // Create data graphs if we have the same target and product nuclei
  for (iExpt = 0; iExpt < nExpt; iExpt++) {

    dataTree->GetEntry(iExpt);

    TString dataName = this->getDataName(dataZ, dataA, iExpt);
    TGraphErrors* dataGraph = this->getGraph(dataName);

    for (iP = 0; iP < nPoints; iP++) {

      cout<<"Setting point "<<dataE[iP]<<" +- "<<dataEErr[iP]
	  <<"; "<<dataSigma[iP]<<" +- "<<dataSigErr[iP]<<endl;

      dataGraph->SetPoint(iP, dataE[iP], dataSigma[iP]);
      dataGraph->SetPointError(iP, dataEErr[iP], dataSigErr[iP]);
    }

  }

  dataFile->Close();

  // Retrieve the calculated graph we want to plot
  // Then overlay this with the measurement points (graphs)
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  TCanvas* theCanvas = new TCanvas("theCanvas", "", 700, 700);
  theCanvas->UseCurrentStyle();
  theCanvas->cd(1);

  Int_t nCalc = (Int_t) _calcNames.size();
  Int_t nData = (Int_t) _dataNames.size();
  cout<<"There are "<<nCalc<<" calc and "<<nData<<" data graphs "<<endl;
  Int_t iCalc, iData;

  for (iCalc = 0; iCalc < nCalc; iCalc++) {

    TString calcName = _calcNames[iCalc];
    cout<<"calcName = "<<calcName<<endl;

    TGraphErrors* calcGraph = this->getGraph(calcName);

    // For each calculation graph, create a null histogram to overlay
    // the graph points on.

    Int_t nCalcPoints = (Int_t) calcGraph->GetN();
    cout<<"nCalcPoints = "<<nCalcPoints<<endl;
    Int_t iP;
    Double_t minCalcE(1e6), maxCalcE(0.0);
    Double_t minCalcSig(1e6), maxCalcSig(0.0);
    for (iP = 0; iP < nCalcPoints; iP++) {

      Double_t tmpEnergy, tmpSigma;
      calcGraph->GetPoint(iP, tmpEnergy, tmpSigma);

      //cout<<"iP = "<<iP<<", E, sigma = "<<tmpEnergy<<" and "<<tmpSigma<<endl;

      if (tmpEnergy < minCalcE) {minCalcE = tmpEnergy;}
      if (tmpEnergy > maxCalcE) {maxCalcE = tmpEnergy;}
      if (tmpSigma > 1e-30 && tmpSigma < minCalcSig) {
	minCalcSig = tmpSigma;
      }
      if (tmpSigma > maxCalcSig) {maxCalcSig = tmpSigma;}
    }

    cout<<"0: minCalcSig = "<<minCalcSig<<endl;
    // Scale limits for energies and cross-sections to nearest powers of ten
    if (_yMin > -1.0) {minCalcSig = _yMin;}
    if (_yMax > -1.0) {maxCalcSig = _yMax;}

    this->scaleLimits(minCalcE, maxCalcE);
    this->scaleLimits(minCalcSig, maxCalcSig);
    // Since we're using a log scale, set min sigma = 1 instead of zero
    cout<<"minCalcSig = "<<minCalcSig<<endl;
    if (fabs(minCalcSig) < 1e-30) {minCalcSig = 1.0;}
    cout<<"minCalcSig = "<<minCalcSig<<endl;

    cout<<"Energy range = "<<minCalcE<<" and "<<maxCalcE<<endl;
    TH2D* null = new TH2D("null", "", 2, minCalcE, maxCalcE, 2, minCalcSig, maxCalcSig);
    null->SetXTitle("E (MeV)");
    null->SetYTitle("#sigma (mb)");
    null->SetTitleOffset(1.25, "X");
    null->SetTitleOffset(1.25, "Y");

    gPad->SetLogx();
    if (_yLog == 1) {gPad->SetLogy();}
    null->Draw();

    //cout<<"Energy range = "<<minCalcE<<" and "<<maxCalcE<<endl;
    //cout<<"Sigma limits = "<<minCalcSig<<" and "<<maxCalcSig<<endl;
    calcGraph->Draw("c");

//     TString legendName(calcName);
//     legendName.ReplaceAll("Calc", "");
//     legendName.ReplaceAll("_", " ");
    //TLegend* legend = new TLegend(0.8, 0.8, 1.0, 1.0, legendName);
    //TLegend* legend = new TLegend(0.75, 0.9, 0.9, 1.0, legendName);

    TString legendName("");
    Double_t xl1(0.65), yl1(0.2), xl2(0.85), yl2(0.4);
    if (_iFlag == 1) {
      xl1 = 0.7, yl1 = 0.7, xl2 = 0.9, yl2 = 0.9;
    } else if (_iFlag == 2) {
      xl1 = 0.7, yl1 = 0.575, xl2 = 0.9, yl2 = 0.9;
    } else if (_iFlag == 3) {
      xl1 = 0.6, yl1 = 0.15, xl2 = 0.8, yl2 = 0.6;
    } else if (_iFlag == 4) {
      xl1 = 0.475, yl1 = 0.13, xl2 = 0.7, yl2 = 0.57;
    }
    TLegend* legend = new TLegend(xl1, yl1, xl2, yl2, legendName);
    legend->SetFillColor(0);
    legend->AddEntry(calcGraph, "Calculated", "l");

    for (iData = 0; iData < nData; iData++) {

      TString dataName = _dataNames[iData];
      cout<<"dataName = "<<dataName<<" for iData = "<<iData<<endl;

      TGraphErrors* dataGraph = this->getGraph(dataName);

      // Make sure that the calc and data graphs are for the same
      // target and product Z, A values. Do this by comparing their names.
      Bool_t doPlot = this->compareNames(calcName, dataName);

      if (doPlot == kTRUE) {

	Int_t iMarker = this->getMarkerStyle(iData);
	Int_t iColour = this->getMarkerColour(iData);

	dataGraph->SetMarkerStyle(iMarker);
	dataGraph->SetMarkerColor(iColour);
	dataGraph->Draw("p");

	TString authorName = _authorNames[iData];
	legend->AddEntry(dataGraph, authorName, "p");

      } else {

	cout<<"doPlot is false for calcName "<<calcName
	    <<" and dataName " <<dataName<<endl;

      }

    } // iData loop

    legend->Draw();
    // Also draw letter specifying figure label
    TText text;
    text.SetTextSize(0.035);
    TString label("");
    if (_iLabel == 1) {
      label = "(a)";
    } else if (_iLabel == 2) {
      label = "(b)";
    } else if (_iLabel == 3) {
      label = "(c)";
    } else if (_iLabel == 4) {
      label = "(d)";
    }
    //text.DrawTextNDC(0.15, 0.85, label);
    cout<<"PlotFileName = "<<plotFileName<<endl;
    theCanvas->Print(plotFileName);

    delete legend;

  } // iCalc loop

  delete theCanvas;

}

void analysis::storeData(const char* rootFileName) {

  // Read the data files containing the measurements of cross-section
  // versus energy and store all of the numbers in an internal ntuple.
  // This ntuple can them be used to plot the data points on top of
  // the various sigma-energy graphs from the Activia output.
  cout<<"Storing data measurements in the ROOT file "<<rootFileName<<endl;

  // Create ROOT file
  TFile* theFile = new TFile(rootFileName, "recreate");
  TTree* theTree = new TTree("Data", "Data");
  theTree->SetDirectory(theFile);
  TString targetName, authorName;
  Double_t Z, A;
  Int_t nPoints;

  // Need to set some default array size to initialise memory use
  // The actual number of values stored is given by the nPoints variable.
  Double_t EArray[1000];
  Double_t EErrArray[1000];
  Double_t sigmaArray[1000];
  Double_t sigmaErrArray[1000];
  Int_t iExpt;
  theTree->Branch("Z", &Z, "Z/D");
  theTree->Branch("A", &A, "A/D");
  theTree->Branch("iExpt", &iExpt, "iExpt/I");
  theTree->Branch("nPoints", &nPoints, "nPoints/I");
  theTree->Branch("E", EArray, "E[nPoints]/D");
  theTree->Branch("EErr", EErrArray, "EErr[nPoints]/D");
  theTree->Branch("sigma", sigmaArray, "sigma[nPoints]/D");
  theTree->Branch("sigmaErr", sigmaErrArray, "sigmaErr[nPoints]/D");

  // List of the data file names containing the cross-section measurements
  // Data files are created using the extractData.pl perl script.
  Int_t iP;
  Double_t energy, energyErr, sigma, sigmaErr;
  vector<Double_t> EVect, EErrVect, sigmaVect, sigmaErrVect;

  _authorNames.clear();

  std::string whiteSpace(" ");

  for (iExpt = 0; iExpt < _nDataFiles; iExpt++) {

    TString dataFileName = _dataFileNames[iExpt];
    cout<<"dataFileName = "<<dataFileName<<endl;

    std::ifstream getData(dataFileName.Data());
    EVect.clear(); sigmaVect.clear();
    EErrVect.clear(); sigmaErrVect.clear();

    // First line should contain the Z and A value of the product
    getData >> targetName >> Z >> A >> authorName;

    cout<<"TargetName = "<<targetName<<" and product Z, A = "<<Z<<" and "<<A<<endl;

    // Capitalise only the first word in the author name
    authorName.ToLower();
    char* authorString = (char*) authorName.Data();
    if (authorString != 0) {*authorString = toupper(*authorString);}
    cout<<"Author name = "<<authorName<<endl;
    // Put the author name in the internal vector
    _authorNames.push_back(authorName);

    while (getData.good()) {

      // Skip comment lines
      if (getData.peek() == '\n') {

	char c;
	getData.get(c);

	if (getData.eof()) {break;}

      } else if (getData.peek() == '#') {

	getData.ignore(1000, '\n');
	getData.putback('\n');

	if (getData.eof()) {break;}

      } else {

	char line[100];
	getData.getline(line, 100);

	if (getData.eof()) {break;}

	std::string lineStr(line);
	vector<std::string> lineVect = this->splitString(lineStr, 
							 whiteSpace);
	int nVect = (int) lineVect.size();

	energy = 0.0; energyErr = 0.0; sigma = 0.0; sigmaErr = 0.0;
	if (nVect == 2) {
	  energy = atof(lineVect[0].c_str());
	  sigma = atof(lineVect[1].c_str());
	} else if (nVect == 4) {
	  energy = atof(lineVect[0].c_str());
	  energyErr = atof(lineVect[1].c_str());
	  sigma = atof(lineVect[2].c_str());
	  sigmaErr = atof(lineVect[3].c_str());
	  cout<<"sigma = "<<sigma<<" +- "<<sigmaErr<<endl;
	}

	EVect.push_back(energy);
	EErrVect.push_back(energyErr);
	sigmaVect.push_back(sigma);
	sigmaErrVect.push_back(sigmaErr);

      }

    }

    nPoints = (Int_t) EVect.size();
    cout<<"nPoints = "<<nPoints<<endl;

    for (iP = 0; iP < nPoints; iP++) {
      EArray[iP] = EVect[iP];
      EErrArray[iP] = EErrVect[iP];
      sigmaArray[iP] = sigmaVect[iP];
      sigmaErrArray[iP] = sigmaErrVect[iP];
      cout<<"E = "<<EArray[iP]<<" +- "<<EErrArray[iP]<<", sigma = "<<sigmaArray[iP]
	  <<" +- "<<sigmaErrArray[iP]<<endl;
    }

    theTree->Fill();

  } // Loop over data files

  // Write the measurement data
  theFile->cd();
  theTree->Write();
  theFile->Close();

}

void analysis::getCalcGraphs(const char* inFileName, const char* ntupleName) {

  // Create graphs of sigma vs energy for all product isotopes
  TFile* theFile = new TFile(inFileName, "read");
  TTree* theTree = (TTree*) theFile->Get(ntupleName);

  Double_t Zt, At, Z, A, frac;
  Int_t nPoints = (Int_t) theTree->GetMaximum("nPoints");
  Double_t E[nPoints], sigma[nPoints];

  Double_t minSigma = theTree->GetMinimum("Sigma");
  Double_t maxSigma = theTree->GetMaximum("Sigma");
  // Scale limits for sigma values to nearest powers of ten
  this->scaleLimits(minSigma, maxSigma);

  theTree->SetBranchAddress("Zt", &Zt);
  theTree->SetBranchAddress("At", &At);
  theTree->SetBranchAddress("Z", &Z);
  theTree->SetBranchAddress("A", &A);
  theTree->SetBranchAddress("frac", &frac);
  theTree->SetBranchAddress("Energy", E);
  theTree->SetBranchAddress("Sigma", sigma);

  Int_t nEvents = theTree->GetEntries();
  Int_t i, iP;
  
  // Loop over the events (i.e. target-product pairs)
  // Target will always have the same Zt, but could have different
  // At values, with appropriate weighting fractions.
  // There could be several product isotopes with different Z and A values.
  // For each product isotope, add the sigma using the weighed fraction.
  for (i = 0; i < nEvents; i++) {

    // Get the ith target-product pair
    theTree->GetEntry(i);

    // Get the name of the product isotope for this event
    TString graphName = this->getCalcName(Z, A);
    cout<<"Name = "<<graphName<<", Target has "<<Zt<<" and "<<At<<endl;
    // Retrieve the graph for this product
    // Creates a new one if it does not already exist.
    TGraphErrors* graph = this->getGraph(graphName);

    // Loop over the number of energy points, storing the
    // energy and weighted sigma in the above graph
    for (iP = 0; iP < nPoints; iP++) {

      // Update the sigma (and energy) point using the weighted
      // target isotope fraction
      // First, retrieve the data point for this product isotope
      Double_t sigmaVal(0.0), EVal(0.0);
      graph->GetPoint(iP, EVal, sigmaVal);

      // Add the sigma contribution from the weighted target isotope
      sigmaVal += sigma[iP]*frac;
      // Replace the (sigma,E) point with the updated sigma value.
      // Not very efficient, but avoids using other vectors/maps...
      graph->SetPoint(iP, E[iP], sigmaVal);

    }

  } // Loop over all target-product pairs

  theFile->Close();

}

void analysis::scaleLimits(Double_t& min, Double_t& max) {

  cout<<"Original limits: "<<min<<" and "<<max<<endl;
  Double_t log10min = log10(min);
  log10min = floor(log10min);
  min = pow(10.0, log10min);

  Double_t log10max = log10(max);
  cout<<"log10max = "<<log10max<<endl;
  Double_t newlog10max = ceil(log10max);
  cout<<"log10max = "<<newlog10max<<endl;
  if ((newlog10max-log10max)/newlog10max < 0.1) {newlog10max += 0.1;}
  max = pow(10.0, newlog10max);
  min *= 0.995;
  cout<<"Scaled limits: "<<min<<" and "<<max<<endl;

}

void analysis::getMinMaxLimits(Double_t& lowValue, Double_t& highValue,
			       Double_t& min, Double_t& max) {

  min = std::min(lowValue, min);
  max = std::max(highValue, max);

}

TGraphErrors* analysis::getGraph(TString& graphName) {

  TGraphErrors* graph(0);

  mapSG::iterator iter = _sigmaEGraphs.find(graphName);
  if (iter != _sigmaEGraphs.end()) {
    graph = iter->second;
  }

  if (graph == 0) {

    cout<<"Creating sigmaEGraph for "<<graphName<<endl<<endl;
    graph = new TGraphErrors();
    graph->SetName(graphName);
    graph->SetTitle(graphName);
    // Insert the new graph into the map
    _sigmaEGraphs[graphName] = graph;

  }

  return graph;

}

TString analysis::getCalcName(Double_t& Z, Double_t& A) {

  TString calcName = this->getName(Z, A);
  calcName += "_Calc";

  // Insert the name into the internal vector
  // First check whether the name is already in the vector
  Bool_t gotName(kFALSE);

  Int_t nNames = (Int_t) _calcNames.size();
  Int_t i;
  for (i = 0; i < nNames; i++) {

    TString name = _calcNames[i];

    if (!calcName.CompareTo(name, TString::kExact)) {
      gotName = kTRUE;
      break;
    }

  }

  if (gotName == kFALSE) {
    cout<<"Adding calc name "<<calcName<<endl;
    _calcNames.push_back(calcName);
  }

  return calcName;

}


TString analysis::getDataName(Double_t& Z, Double_t& A, Int_t& iExpt) {

  TString dataName = this->getName(Z, A);
  dataName += "_Data"; dataName += iExpt;

  // Insert the name into the internal vector
  // First check whether the name is already in the vector
  Bool_t gotName(kFALSE);

  Int_t nNames = (Int_t) _dataNames.size();
  Int_t i;
  for (i = 0; i < nNames; i++) {

    TString name = _dataNames[i];

    if (!dataName.CompareTo(name, TString::kExact)) {
      gotName = kTRUE;
      break;
    }

  }

  if (gotName == kFALSE) {
    cout<<"Adding data name "<<dataName<<endl;
    _dataNames.push_back(dataName);
  }

  return dataName;

}

TString analysis::getName(Double_t& Z, Double_t& A) {

  TString name(_targetName); name += "_";
  name += this->getElementName(Z);
  //name += "";
  Double_t AIntPart = 0.0;
  Double_t AFracPart = modf(A, &AIntPart);

  name += (Int_t) AIntPart; 

  TString AFracString("");
  if (AFracPart > 0.0) {
    AFracString += AFracPart;
    AFracString.ReplaceAll("0.", "");
    AFracString.ReplaceAll(" ", "");
    name += "_"; name += AFracString;
  }

  return name;
}

Bool_t analysis::compareNames(TString& calcName, TString& dataName) {

  Bool_t sameNames(kFALSE);

  TString newCalcName(calcName);
  newCalcName = newCalcName.ReplaceAll("_Calc", "");

  TString newDataName(dataName);
  Int_t index = newDataName.Index("_Data");
  newDataName.Resize(index);

  if (!newDataName.CompareTo(newCalcName, TString::kExact)) {
    sameNames = kTRUE;
  }

  return sameNames;

}

TString analysis::getElementName(Double_t& Z) {

  TString elementName("");
  Int_t intZ = (Int_t) Z;

  // Loop over element name map
  mapElem::iterator iter = _elements.find(intZ);
  if (iter != _elements.end()) {
    elementName = iter->second;
  }

  return elementName;

}

void analysis::storeElements(const char* listOfElements) {

  _elements.clear();

  Int_t Z;
  TString name;

  std::ifstream getData(listOfElements);

  while (getData.good()) {

    getData >> Z >> name;

    if (getData.eof()) {break;}

    _elements[Z] = name;

  }

}

void analysis::selectMarkers() {

  _markerStyles.clear();
  _markerColours.clear();

  const Int_t nTypes = 15;
  _markerStyles.resize(nTypes);
  _markerColours.resize(nTypes);

  // See TAttFill for marker styles and colours
  //Int_t styles[nTypes] = {3, 20, 21, 22, 23, 5, 24, 25, 26, 28, 5, 24, 25, 26, 28};
  Int_t styles[nTypes] = {3, 20, 21, 22, 23, 5, 24, 25, 26, 28, 5, 20, 21, 22, 23};

  //Int_t styles[nTypes]  = {5, 20, 21, 22, 23, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34};
  //Int_t colours[nTypes] = {4, 6, 8, 9, 12, 26, 28, 36, 38, 46, 3, 7, 29, 30, 41};
  //Int_t colours[nTypes] = {1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4, 1, 2, 4};

  Int_t colours[nTypes] = {1, 1, 1, 1, 1, 4, 4, 4, 4, 4, 6, 6, 6, 6, 6};

  Int_t i;
  for (i = 0; i < nTypes; i++) {
    _markerStyles[i] = styles[i];
    _markerColours[i] = colours[i];
  }

}

Int_t analysis::getMarkerStyle(Int_t& index) {

  Int_t iStyle(0);
  Int_t nStyles = (Int_t) _markerStyles.size();

  Int_t modIndex = index%nStyles;
  iStyle = _markerStyles[modIndex];

  return iStyle;

}

Int_t analysis::getMarkerColour(Int_t& index) {

  Int_t iColour(0);
  Int_t nColours = (Int_t) _markerColours.size();

  Int_t modIndex = index%nColours;
  iColour = _markerColours[modIndex];
  
  return iColour;

}

vector<std::string> analysis::splitString(std::string& origString,
					  std::string& splitter) {

  // Code from STLplus
  vector<std::string> result;

  if (!origString.empty() && !splitter.empty()) {
    
    for (std::string::size_type offset = 0;;) {

      std::string::size_type found = origString.find(splitter, offset);

      if (found != std::string::npos) {
	std::string tmpString = origString.substr(offset, found-offset);
	if (tmpString.size() > 0) {result.push_back(tmpString);}
        offset = found + splitter.size();
      } else {
	std::string tmpString = origString.substr(offset, origString.size()-offset);
	if (tmpString.size() > 0) {result.push_back(tmpString);}
        break;
      }
    }
  }

  return result;

}

int main(const int argc, const char** argv) {

  if (argc < 2 || argc > 7) {
    cout<<"Expecting: ./analysis <controlFile> <yLog> <yMin> <yMax> <iFlag> <iLabel>"<<endl;
    return -1;
  }

  const char* controlFileName = argv[1];
  int yLog(1);
  if (argc == 3) {yLog = atoi(argv[2]);}
  double yMin(-1.0), yMax(-1.0);
  if (argc > 3) {
    yMin = atof(argv[3]);
  }
  if (argc > 4) {
    yMax = atof(argv[4]);
  }

  int iFlag(0);
  if (argc > 5) {
    iFlag = atoi(argv[5]);
  }

  int iLabel(0);
  if (argc > 6) {
    iLabel = atoi(argv[6]);
  }

  analysis a(yLog, yMin, yMax, iFlag, iLabel);
  a.run(controlFileName);

  return 0;

}
