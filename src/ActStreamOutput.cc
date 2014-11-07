// Class for output data to an output text file

#include "Activia/ActStreamOutput.hh"
#include "Activia/ActGraphPoint.hh"
#include "Activia/ActOutputSelection.hh"

#include <vector>

using std::cout;
using std::endl;

ActStreamOutput::ActStreamOutput(const char* fileName, int levelOfDetail) : ActAbsOutput(fileName, levelOfDetail)
{
  // Constructor
  _precision = 6;
  _minWidth = 7;
  _stream.flags(std::ios::left);
  _stream.precision(_precision);
  _type = ActOutputSelection::Stream;
  _typeName = "Stream";
}

ActStreamOutput::~ActStreamOutput() 
{
  // Destructor

}

void ActStreamOutput::openFile() {

  _stream.open(_fileName.c_str());

}

void ActStreamOutput::setOptions() {

}

void ActStreamOutput::outputLineOfText(const char* line) {
  
  _stream << line << endl;

}

void ActStreamOutput::outputLineOfText(std::string& line) {

  _stream << line << endl;

}

void ActStreamOutput::outputLineOfText(ActString& line) {
  
  _stream << line.getData() << endl;

}

void ActStreamOutput::outputLineOfText(std::vector<ActString>& strings) {

  int nStrings = (int) strings.size();
  int i;

  for (i = 0; i < nStrings; i++) {
    ActString s = strings[i];
    int nS = s.size() + 1;
    _stream.width(nS); _stream << s.getData();
  }

  _stream << endl;

}

void ActStreamOutput::outputTable(ActOutputTable& table) {

  int nColumns = table.getNColumns();
  std::vector<ActString> columnNames = table.getColumnNames();
  std::vector<int> colWidths(nColumns);
  int iCol;

  int minWidth = table.getColumnSpacing();

  _stream.precision(_precision);

  // Print out column names first, then the data row by row
  for (iCol = 0; iCol < nColumns; iCol++) {

    ActString column = columnNames[iCol];
    int width = column.size() + 1;
    if (width < minWidth) {width = minWidth;}

    colWidths[iCol] = width;

    std::string name = column.getString();
    _stream.width(width); _stream << name;

  }

  _stream << endl;

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

      int width = colWidths[iCol];
      double value = row[iCol];
      _stream.width(width); _stream << value;

    }

    _stream << endl;

  } // Loop over rows
  
}

void ActStreamOutput::outputGraph(ActNucleiData& data,
				  ActAbsGraph& graph) {

  double zt = data.getzt();
  double at = data.getat();
  double z = data.getz();
  double a = data.geta();

  // Print out the (Z,A) columns of the target and product nuclei
  int zWidth = 7;
  _stream.width(zWidth); _stream << "Z(tgt)";
  _stream.width(zWidth); _stream << "A(tgt)";
  _stream.width(zWidth); _stream << "Z(pro)";
  _stream.width(zWidth); _stream << "A(pro)";

  // Retrieve names of the x and y axes
  std::string xAxis = graph.getXAxisName();
  std::string xUnits = graph.getXAxisUnits();

  ActString xString(xAxis); xString += " (";
  xString += xUnits; xString += ")";
  int xWidth = xString.size() + 1;
  _stream.width(xWidth); _stream << xString.getData();

  int nYAxes = graph.getnYAxes();
  std::vector<std::string> yAxesNames = graph.getYAxesNames();
  std::vector<std::string> yAxesUnits = graph.getYAxesUnits();
  std::vector<double> yTotals(nYAxes);
  std::vector<int> yWidths(nYAxes);
  int iY;

  for (iY = 0; iY < nYAxes; iY++) {

    yTotals[iY] = 0.0;
    std::string yAxis = yAxesNames[iY];
    std::string yUnits = yAxesUnits[iY];

    ActString yString(yAxis); yString += " (";
    yString += yUnits; yString += ")";
    int ySize = yString.size() + 1;
    yWidths[iY] = ySize;
    _stream.width(ySize); _stream << yString.getData();

  }

  _stream << endl;

  // Write out the (Z,A) values for the target and product nuclei
  _stream.width(zWidth); _stream << zt;
  _stream.width(zWidth); _stream << at;
  _stream.width(zWidth); _stream << z;
  _stream.width(zWidth); _stream << a;

  // Get the vector of points for this graph
  std::vector<ActGraphPoint> points = graph.getPoints();
  int nPoints = (int) points.size();
  int iX;
  int emptyWidth = 4*zWidth;

  // Loop over all points
  for (iX = 0; iX < nPoints; iX++) {

    if (iX != 0) {_stream.width(emptyWidth); _stream << "";}

    // Write out the x point value
    ActGraphPoint point = points[iX];
    double xVar = point.getX();

    _stream.width(xWidth); 
    _stream.precision(_precision);
    _stream << xVar;

    // Retrieve and loop over all y values
    std::vector<double> yValues = point.getYValues();
    int nYValues = point.getnYValues();    

    for (iY = 0; iY < nYValues; iY++) {

      double yValue = yValues[iY];
      _stream.width(yWidths[iY]);
      _stream.precision(_precision);
      _stream << yValue;

      yTotals[iY] += yValue;

    }

    _stream << endl;

  }

  _stream.width(emptyWidth+xWidth); _stream << "Totals:";
  for (iY = 0; iY < nYAxes; iY++) {
    _stream.width(yWidths[iY]); 
    _stream << yTotals[iY];
  }

  _stream << endl;

}

void ActStreamOutput::closeFile() {
  _stream.close();
}
