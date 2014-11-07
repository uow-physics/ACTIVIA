#ifdef ACT_USE_ROOT

#ifndef ACT_ROOT_OUTPUT_HH
#define ACT_ROOT_OUTPUT_HH

#include "Activia/ActAbsOutput.hh"
#include "Activia/ActAbsGraph.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActString.hh"
#include "Activia/ActOutputTable.hh"

#include <string>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

class ActInput;

/// \brief Store output from the calculations as a ROOT file

class ActROOTOutput : public ActAbsOutput {

 public:

  /// Constructor, specifying the ROOT output file name and level of detail
  ActROOTOutput(const char* fileName, int levelOfDetail = 0);
  /// Destructor
  virtual ~ActROOTOutput();

  /// Open the output file
  virtual void openFile();
  /// Write a line of text to the output file - this will be printed out using cout
  virtual void outputLineOfText(const char* line);
  /// Write a line of text to the output file - this will be printed out using cout
  virtual void outputLineOfText(std::string& line);
  /// Write a line of text to the output file - this will be printed out using cout
  virtual void outputLineOfText(ActString& line);
  /// Write a line of text to the output file - this will be printed out using cout
  virtual void outputLineOfText(std::vector<ActString>& strings);

  /// Write out a table of results to the output file
  virtual void outputTable(ActOutputTable& table);

  /// Write out a graph of results and related nuclei data to the appropriate ntuple
  virtual void outputGraph(ActNucleiData& data,
			   ActAbsGraph& graph);
  /// Close the output file
  virtual void closeFile();

  /// Set any writing options
  virtual void setOptions();

  /// Find the ntuple with the given name
  TTree* findNtuple(TString& ntupleName);

 protected:
  
 private:

  TFile* _theFile;
  std::vector<TTree*> _theTrees;
  TTree* _currentTree;
  int _nTrees;

};

#endif

#endif
