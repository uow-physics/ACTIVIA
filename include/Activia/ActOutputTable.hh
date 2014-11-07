#ifndef ACT_OUTPUT_TABLE_HH
#define ACT_OUTPUT_TABLE_HH

#include "Activia/ActString.hh"

#include <vector>
#include <string>

/// \brief A class for defining an output table
///
/// This stores column names and rows of doubles in a table format.

class ActOutputTable {

 public:

  /// Construct an empty table with a given name
  ActOutputTable(const char* name);
  /// Construct a table with the provided list of column names. Column width set to 14.
  ActOutputTable(const char* name, std::vector<ActString>& columnNames);
  /// Construct a table with the provided list of column names and specify the spacing between the columns.
  ActOutputTable(const char* name, std::vector<ActString>& columnNames, int columnSpacing);

  virtual ~ActOutputTable();

  /// Set the names of the columns
  void setColumnNames(std::vector<ActString>& columnNames);

  /// Add a row of numbers. The number of values in the row should match the number of columns.
  void addRow(std::vector<double>& values);

  /// Get all available rows of data
  std::vector< std::vector <double> > getRows() {return _rows;}
  /// Get the specific row of data
  std::vector<double> getRow(int index);
  /// Get all of the column names
  std::vector<ActString> getColumnNames() {return _columnNames;}

  /// Return the name of the table.
  std::string getName() {return _name;}

  /// Get the text width spacing between columns.
  int getColumnSpacing() {return _colWidth;}

  /// Get the number of columns
  int getNColumns() {return _nColumns;}

  /// Reset the table. Clears all rows and column names. Column width set to 14.
  void reset();

 protected:

 private:

  std::string _name;
  std::vector<ActString> _columnNames;
  std::vector<std::vector <double> > _rows;

  int _nColumns, _colWidth;

};

#endif
