// Class for defining a table of output

#include "Activia/ActOutputTable.hh"

#include <iostream>
using std::cout;
using std::endl;

ActOutputTable::ActOutputTable(const char* name)
{
  // Constructor
  _name = std::string(name);
  this->reset();
}

ActOutputTable::ActOutputTable(const char* name, std::vector<ActString>& columnNames) 
{
  // Constructor
  _name = std::string(name);
  this->reset();
  this->setColumnNames(columnNames);

}

ActOutputTable::ActOutputTable(const char* name, std::vector<ActString>& columnNames, int columnSpacing) 
{
  // Constructor
  _name = std::string(name);
  this->reset();
  this->setColumnNames(columnNames);
  _colWidth = columnSpacing;

}

ActOutputTable::~ActOutputTable() 
{
  // Destructor
  this->reset();
}

void ActOutputTable::reset() { 

  _columnNames.clear();
  int i;
  int nRows = _rows.size();
  for (i = 0; i < nRows; i++) {
    _rows[i].clear();
  }
  _rows.clear();
  _nColumns = 0;
  _colWidth = 14;
}

void ActOutputTable::setColumnNames(std::vector<ActString>& columnNames) {

  _columnNames.clear();
  _nColumns = (int) columnNames.size();
  int i;
  for (i = 0; i < _nColumns; i++) {
    ActString column = columnNames[i];
    _columnNames.push_back(column);
  }
}

void ActOutputTable::addRow(std::vector<double>& values) {

  int nValues = (int) values.size();
  if (nValues != _nColumns) {
    cout<<"Error in ActOutputTable::addRow. Number of values "<<nValues
	<<" does not match the number of columns "<<_nColumns<<endl;
    return;
  }

  _rows.push_back(values);

}

std::vector<double> ActOutputTable::getRow(int index) {

  std::vector<double> row;
  if (index >= 0 && index < _nColumns-1) {row = _rows[index];}

  return row;

}
