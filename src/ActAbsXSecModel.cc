// Abstract class for cross section empirical models/formulae

#include "Activia/ActAbsXSecModel.hh"

ActAbsXSecModel::ActAbsXSecModel(std::string name, int debug) : 
  _name(name), _sigma(0.0), _formulae(), _debug(debug)
{
  // Constructor
}

ActAbsXSecModel::~ActAbsXSecModel() 
{
  // Destructor
}
