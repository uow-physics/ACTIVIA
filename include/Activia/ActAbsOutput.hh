#ifndef ACT_ABS_OUTPUT_HH
#define ACT_ABS_OUTPUT_HH

#include "Activia/ActAbsGraph.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActString.hh"
#include "Activia/ActOutputTable.hh"
#include "Activia/ActAbsCalcStatus.hh"

#include <string>
#include <vector>

/// \brief Abstract class for output data
///
/// Classes inherting from this must implement various functions to specify
/// how data is written to a given output file.

class ActAbsOutput {

 public:

  /// Constructor providing the output file name and the level of detail
  ActAbsOutput(const char* fileName = "", int levelOfDetail = 0);
  ActAbsOutput(ActAbsCalcStatus* calcStatus, const char* fileName = "", int levelOfDetail = 0);
  virtual ~ActAbsOutput();
  
  /// Method to implement opening the output file
  virtual void openFile() = 0;

  // Methods to print out a line of text
  /// Output a line of text
  virtual void outputLineOfText(const char* line) = 0;
  /// Output a line of text
  virtual void outputLineOfText(std::string& line) = 0;
  /// Output a line of text
  virtual void outputLineOfText(ActString& line) = 0;
  /// Output a line of text
  virtual void outputLineOfText(std::vector<ActString>& strings) = 0;

  /// Method to print out a table of numbers
  virtual void outputTable(ActOutputTable& table) = 0;

  /// Method to print out data from a graph with information about
  /// a target-product nuclide pair
  virtual void outputGraph(ActNucleiData& data,
			   ActAbsGraph& graph) = 0;

  /// Method to finalise and close the output file
  virtual void closeFile() = 0;

  /// Set any options for the output file/class
  virtual void setOptions() {;}
  
  /// Set the level of detail
  void setLevelOfDetail(int detail) {_detail = detail;}
  /// Retrieve the level of detail
  int  getLevelOfDetail() {return _detail;}

  /// Set the output type
  void setType(int type) {_type = type;}
  /// Get the output type
  int getType() {return _type;}

  /// Get the type name
  std::string getTypeName() {return _typeName;}
  /// Get the output file name.
  std::string getFileName() {return _fileName;}

  /// Set the calculation status pointer
  void setCalcStatus(ActAbsCalcStatus* calcStatus) {_calcStatus = calcStatus;}

  ActAbsCalcStatus* getCalcStatus() {return _calcStatus;}

 protected:

  /// The filename of the output
  std::string _fileName;
  /// Type and detail of the output
  int _type, _detail;
  /// Class (type) name
  std::string _typeName;
  /// Pointer to current calculation status for debugging purposes/GUI etc..
  ActAbsCalcStatus* _calcStatus;

 private:

};

#endif
