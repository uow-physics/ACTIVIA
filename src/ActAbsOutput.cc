// Abstract class for outputing data

#include "Activia/ActAbsOutput.hh"

ActAbsOutput::ActAbsOutput(const char* fileName, int levelOfDetail) : _fileName(fileName), 
								      _type(0), _detail(levelOfDetail),
								      _typeName(""), _calcStatus(0)
{
  // Constructor
}

ActAbsOutput::ActAbsOutput(ActAbsCalcStatus* calcStatus, const char* fileName, int levelOfDetail)
  : _fileName(fileName), 
    _type(0), _detail(levelOfDetail),
    _typeName(""), _calcStatus(calcStatus)
{
  // Constructor
}

ActAbsOutput::~ActAbsOutput() 
{
  // Destructor
}
