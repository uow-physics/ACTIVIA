// Simple class to define the exposure and decay times

#include "Activia/ActTime.hh"

ActTime::ActTime(double exposureTime, double decayTime) : ActAbsData()
{
  // Constructor
  _exposureTime = exposureTime;
  _decayTime = decayTime;
}

ActTime::~ActTime() 
{
  // Destructor
}
