#ifndef ACT_STREAM_OUTPUT_HH
#define ACT_STREAM_OUTPUT_HH

#include "Activia/ActAbsOutput.hh"
#include "Activia/ActAbsGraph.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActString.hh"
#include "Activia/ActOutputTable.hh"

#include <iostream>
#include <fstream>
#include <string>

class ActInput;

/// \brief A class for writing data to an output ASCII text file.

class ActStreamOutput : public ActAbsOutput {

 public:
  
  /// Define a stream output with the given file name and level of detail
  ActStreamOutput(const char* fileName, int levelOfDetail = 0);
  /// Destructor
  virtual ~ActStreamOutput();

  /// Open the output file
  virtual void openFile();
  /// Write a line of text to the output file
  virtual void outputLineOfText(const char* line);
  /// Write a line of text to the output file
  virtual void outputLineOfText(std::string& line);
  /// Write a line of text to the output file
  virtual void outputLineOfText(ActString& line);
  /// Write a line of text to the output file
  virtual void outputLineOfText(std::vector<ActString>& strings);

  /// Write out a table of results to the output file
  virtual void outputTable(ActOutputTable& table);

  /// Write out a graph of results and related nuclei data to the output file
  virtual void outputGraph(ActNucleiData& data,
			   ActAbsGraph& graph);

  /// Close the output file
  virtual void closeFile();

  /// Set any writing options
  virtual void setOptions();

  /// Set the precision for writing out numbers.
  void setPrecision(int number) {_precision = number; _stream.precision(number);}
  /// Set the minimum total string width for writing out numbers
  void setMinWidth(int number) {_minWidth = number;}

 protected:
  
 private:

  std::ofstream _stream;
  int _precision, _minWidth;

};

#endif
