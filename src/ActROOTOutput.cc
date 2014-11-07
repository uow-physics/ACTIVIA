// Class for output data to ROOT files
#ifdef ACT_USE_ROOT

#include "Activia/ActROOTOutput.hh"
#include "Activia/ActGraphPoint.hh"
#include "Activia/ActOutputSelection.hh"

#include <iostream>
using std::cout;
using std::endl;

ActROOTOutput::ActROOTOutput(const char* fileName, int levelOfDetail) : ActAbsOutput(fileName, levelOfDetail)
{
  // Constructor
  _type = ActOutputSelection::ROOT;
  _typeName = "ROOT";
  _theFile = 0; _theTrees.clear(); 
  _currentTree = 0; _nTrees = 0;
}

ActROOTOutput::~ActROOTOutput() 
{
  // Destructor
  if (_theFile != 0 && _theFile->IsOpen()) {
    _theFile->Close();
  }

}

void ActROOTOutput::openFile() {

  if (_theFile != 0) {_theFile->Close();}
  _theFile = new TFile(_fileName.c_str(), "recreate");
  _theFile->cd();

}

void ActROOTOutput::setOptions() {

}

void ActROOTOutput::outputLineOfText(const char* line) {
  
  cout << line << endl;

}

void ActROOTOutput::outputLineOfText(std::string& line) {

  cout << line << endl;

}

void ActROOTOutput::outputLineOfText(ActString& line) {
  
  cout << line.getData() << endl;

}

void ActROOTOutput::outputLineOfText(std::vector<ActString>& strings) {

  int nStrings = (int) strings.size();
  int i;

  for (i = 0; i < nStrings; i++) {
    ActString s = strings[i];
    int nS = s.size() + 1;
    cout.width(nS); cout << s.getData();
  }

  cout << endl;

}

void ActROOTOutput::outputTable(ActOutputTable& table) {

  // A table represents a single "ntuple", where each row
  // corresponds to a new "event". Columns represent the
  // different variables in the table.

  std::string ntupleName = table.getName();
  cout<<"Creating ROOT ntuple "<<ntupleName.c_str()<<endl;

  TTree* theTree = new TTree(ntupleName.c_str(), ntupleName.c_str());
  // Let the tree be written to disk as it is filled.
  theTree->SetDirectory(_theFile);

  int nColumns = table.getNColumns();
  std::vector<ActString> columnNames = table.getColumnNames();

  std::vector<double> varValues(nColumns);

  // Set the branch information
  int iCol;
  for (iCol = 0; iCol < nColumns; iCol++) {

    ActString column = columnNames[iCol];
    std::string varName = column.getString();
    std::string varAndType = varName;
    varAndType += "/D"; // always doubles in the table

    varValues[iCol] = 0.0; // initialise the value
    theTree->Branch(varName.c_str(), &varValues[iCol], varAndType.c_str());

  }

  std::vector< std::vector <double> > rows = table.getRows();
  int nRows = (int) rows.size();
  int iRow;
  for (iRow = 0; iRow < nRows; iRow++) {

    std::vector<double> row = rows[iRow];

    int nCol = (int) row.size();
    if (nCol != nColumns) {
      cout << "Error in outputTable. Row " << iRow << " has " << nCol 
	   << " columns which should be " << nColumns << endl;
      continue;
    }

    for (iCol = 0; iCol < nColumns; iCol++) {

      varValues[iCol] = row[iCol];
    }

    theTree->Fill();

  } // Loop over rows

  // Store the TTree pointer in internal vector. These pointers should
  // all be attached to the file pointer, _theFile.
  _theTrees.push_back(theTree);
  _nTrees = (int) _theTrees.size();

}

TTree* ActROOTOutput::findNtuple(TString& ntupleName) {

  if (_currentTree != 0) {
    TString currentName = _currentTree->GetName();
    if (!currentName.CompareTo(ntupleName, TString::kExact)) {
      return _currentTree;
    }
  }

  TTree* theTree(0);
  // We have a different ntuple.
  int i;
  for (i = 0; i < _nTrees; i++) {

    TTree* tree = _theTrees[i];

    if (tree != 0) {
      const char* name = tree->GetName();
      TString treeName(name);
      if (!treeName.CompareTo(ntupleName, TString::kExact)) {
	theTree = tree; _currentTree = tree;
	break;
      }
    } // Tree is not null      
  } // Loop over available trees

  return theTree;

}

void ActROOTOutput::outputGraph(ActNucleiData& data,
				ActAbsGraph& graph) {

  // Write out the graph of values (e.g. sigma vs energy) 
  // for the given set of nuclei data (Z,A values). The graph is represented
  // by an array of values for the ntuple row defined by the Z and A values from
  // the nuclei data. 

  // Get the name of the graph.
  std::string graphName = graph.getName();
  TString ntupleTString(graphName);
  TTree* theTree = this->findNtuple(ntupleTString);

  double zt = data.getzt();
  double at = data.getat();
  double frac = data.getFraction();
  double z  = data.getz();
  double a  = data.geta();

  int nYAxes = graph.getnYAxes();
  int iY, iPoint;
  
  // Get the number of graph points
  std::vector<ActGraphPoint> points = graph.getPoints();
  int nPoints = (int) points.size();

  // Get the x and y variable (axis) names
  std::string xAxisName = graph.getXAxisName();
  std::vector< std::string > yAxesNames = graph.getYAxesNames();

  // Create arrays to assign the x and y values in the ntuple.
  double xArray[nPoints];
  double yArray[nYAxes][nPoints];
  
  // Loop over all points and fill in the above arrays
  for (iPoint = 0; iPoint < nPoints; iPoint++) {

    // Get the point in the graph
    ActGraphPoint thePoint = points[iPoint];
    // Store the x value in the array
    xArray[iPoint] = thePoint.getX();

    // Retrieve the y values for this point
    std::vector<double> yValues = thePoint.getYValues();
    int nYPoints = thePoint.getnYValues();
    if (nYPoints != nYAxes) {
      cout<<"Error in ActROOTOutput::outputGraph. Number of y values for point "
	  <<iPoint<<" = "<<nYPoints<<" != number of y axes = "<<nYAxes<<endl;
      continue;
    }
    // Fill the y values in the array
    for (iY = 0; iY < nYAxes; iY++) {
      yArray[iY][iPoint] = yValues[iY];
    }
  }

  if (theTree == 0) {
 
    // Create a new tree with the appropriate variables
    theTree = new TTree(ntupleTString, ntupleTString);
    theTree->SetDirectory(_theFile);
    _theTrees.push_back(theTree);
    _nTrees = (int) _theTrees.size();

    theTree->Branch("Zt", &zt, "Zt/D");
    theTree->Branch("At", &at, "At/D");
    theTree->Branch("frac", &frac, "frac/D");
    theTree->Branch("Z", &z, "Z/D");
    theTree->Branch("A", &a, "A/D");

    theTree->Branch("nPoints", &nPoints, "nPoints/I");

    // Assign the branch for the x axis points (double variables)
    std::string xAxisType(xAxisName);
    xAxisType += "[nPoints]/D";
    theTree->Branch(xAxisName.c_str(), xArray, xAxisType.c_str());

    for (iY = 0; iY < nYAxes; iY++) {

      std::string yAxisName = yAxesNames[iY];
      std::string yAxisType(yAxisName); 
      yAxisType += "[nPoints]/D";
      double* yValues = yArray[iY];
      theTree->Branch(yAxisName.c_str(), yValues, yAxisType.c_str());

    }

  }

  // Set the branch addresses for the tree variables and fill in the data
  if (theTree != 0) {

    theTree->SetBranchAddress("Zt", &zt);
    theTree->SetBranchAddress("At", &at);
    theTree->SetBranchAddress("frac", &frac);
    theTree->SetBranchAddress("Z", &z);
    theTree->SetBranchAddress("A", &a);
    theTree->SetBranchAddress("nPoints", &nPoints);
    theTree->SetBranchAddress(xAxisName.c_str(), xArray);
    
    for (iY = 0; iY < nYAxes; iY++) {
      double* yValues = yArray[iY];
      theTree->SetBranchAddress(yAxesNames[iY].c_str(), yValues);
    }

    // Fill in data

    theTree->Fill();

  } else {
    cout<<"Error in ActROOTOutput::outputGraph. Could not find/create the TTree "
	<<graphName<<endl;
  }      

}

void ActROOTOutput::closeFile() {

  _theFile->cd();

  cout<<"Writing ROOT output"<<endl;

  int nTrees = _theTrees.size();
  int i;
  for (i = 0; i < nTrees; i++) {

    TTree* theTree = _theTrees[i];
    if (theTree != 0) {
      cout<<"Writing out TTree "<<theTree->GetName()
	  <<" to the file "<<_fileName<<endl;
      theTree->Write();
    }
  }

  _theFile->Close();
  delete _theFile; _theFile = 0;

}

#endif
