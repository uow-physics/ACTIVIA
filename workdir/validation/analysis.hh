#include "TString.h"
#include "TGraphErrors.h"

#include <vector>
#include <string>
#include <map>

using std::map;
using std::vector;

class analysis {

 public:

  analysis(int yLog = 1, double yMin = -1.0, double yMax = -1.0, 
	   int iFlag = 0, int iLabel = 0);
  virtual ~analysis();

  void run(const char* controlFileName);

  void storeData(const char* rootFileName);

  void plotGraphs(const char* plotFileName);

  void getCalcGraphs(const char* inFileName, 
		     const char* ntupleName = "xSecEData");

  void scaleLimits(Double_t& min, Double_t& max);
  void getMinMaxLimits(Double_t& lowValue, Double_t& highValue,
		       Double_t& min, Double_t& max);

  TGraphErrors* getGraph(TString& prodName);

  TString getCalcName(Double_t& Z, Double_t& A);
  TString getDataName(Double_t& Z, Double_t& A, Int_t& iExpt);
  TString getName(Double_t& Z, Double_t& A);
  Bool_t compareNames(TString& calcName, TString& dataName);

  TString getElementName(Double_t& Z);
  void storeElements(const char* listOfElements = "listOfElements.dat");

  void setTargetName(const char* name) {_targetName = TString(name);}
  TString getTargetName() {return _targetName;}

  void selectMarkers();
  Int_t getMarkerStyle(Int_t& index);
  Int_t getMarkerColour(Int_t& index);

  vector<std::string> splitString(std::string& origString,
				  std::string& splitter);

 private:

  int _yLog;
  double _yMin, _yMax;
  int _iFlag, _iLabel;
  TString _targetName;
  vector<TString> _dataFileNames;
  vector<TString> _authorNames;
  Int_t _nDataFiles;
  TString _dataRootFile;

  typedef map<TString, TGraphErrors*> mapSG;
  mapSG _sigmaEGraphs;
  
  vector<TString> _calcNames, _dataNames;

  typedef map<Int_t, TString> mapElem;
  mapElem _elements;

  vector<Int_t> _markerStyles;
  vector<Int_t> _markerColours;

};
