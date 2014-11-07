// Class to define xSec and yield output files, including format and level of detail
#include "Activia/ActOutputSelection.hh"
#include <iostream>

ActOutputSelection::ActOutputSelection()
{
  // Constructor
  _xSecFileName = ""; _decayFileName = "";
  this->allowedTypes();
  this->allowedDetails();
  _xSecType = _defaultType;
  _decayType = _defaultType;
  _xSecDetail = _defaultDetail;
  _decayDetail = _defaultDetail;
  _xSecExtension = ".out"; _decayExtension = ".out";
}

ActOutputSelection::~ActOutputSelection() 
{
  // Destructor
  _allowedTypes.clear();
  _allowedDetails.clear();
}

void ActOutputSelection::setXSecType(int type) {

  bool ok(false);

  ActOutputMap::iterator iter = _allowedTypes.find(type);

  if (iter != _allowedTypes.end()) {
    _xSecType = iter->first;
    ok = true;
  }

  if (ok == false) {
    std::cout<<"Warning in ActOutputSelection::setXSetType. The type "
	     <<type<<" is not available. Setting to ASCII text (type "
	     <<ActOutputSelection::Stream<<")"<<std::endl;
    _xSecType = ActOutputSelection::Stream;
  }

  _xSecExtension = this->setExtension(_xSecType);

}

void ActOutputSelection::setDecayType(int type) {

  bool ok(false);

  ActOutputMap::iterator iter = _allowedTypes.find(type);

  if (iter != _allowedTypes.end()) {
    _decayType = iter->first;
    ok = true;
  }

  if (ok == false) {
    std::cout<<"Warning in ActOutputSelection::setDecayType. The type "
	     <<type<<" is not available. Setting to ASCII text (type "
	     <<ActOutputSelection::Stream<<")"<<std::endl;
    _decayType = ActOutputSelection::Stream;
  }

  _decayExtension = this->setExtension(_decayType);

}

void ActOutputSelection::setXSecDetail(int type) {
 
  bool ok(false);

  ActOutputMap::iterator iter = _allowedDetails.find(type);

  if (iter != _allowedDetails.end()) {
    _xSecDetail = iter->first;
    ok = true;
  }

  if (ok == false) {
    std::cout<<"Warning in ActOutputSelection::setXSecDetail. The level of detail "
	     <<type<<" is not available. Setting to summary information (detail flag = "
	     <<ActOutputSelection::Summary<<")."<<std::endl;
    _xSecDetail = ActOutputSelection::Summary;
  } 

}

void ActOutputSelection::setDecayDetail(int type) {
 
  bool ok(false);

  ActOutputMap::iterator iter = _allowedDetails.find(type);

  if (iter != _allowedDetails.end()) {
    _decayDetail = iter->first;
    ok = true;
  }

  if (ok == false) {
    std::cout<<"Warning in ActOutputSelection::setDecayDetail. The level of detail "
	     <<type<<" is not available. Setting to summary information (detail flag = "
	     <<ActOutputSelection::Summary<<"),"<<std::endl;
    _decayDetail = ActOutputSelection::Summary;
  } 

}

void ActOutputSelection::allowedTypes() {

  _allowedTypes.clear();

  // ASCII text stream always available
  std::string typeName("ASCII text");

  _allowedTypes[ActOutputSelection::Stream] = typeName;
  _defaultType = ActOutputSelection::Stream;
  _defaultTypeName = typeName;
  _xSecExtension = this->setExtension(ActOutputSelection::Stream);
  _decayExtension = this->setExtension(ActOutputSelection::Stream);

  // ROOT output if appropriate environment is present
#ifdef ACT_USE_ROOT
  typeName = "ROOT";
  _allowedTypes[ActOutputSelection::ROOT] = typeName;
  _defaultType = ActOutputSelection::ROOT;
  _defaultTypeName = typeName;
  _xSecExtension = this->setExtension(ActOutputSelection::ROOT);
  _decayExtension = this->setExtension(ActOutputSelection::ROOT);
#endif

}

void ActOutputSelection::allowedDetails() {

  _allowedDetails.clear();

  _allowedDetails[ActOutputSelection::Summary] = "Summary";
  _allowedDetails[ActOutputSelection::All] = "All";

  _defaultDetail = ActOutputSelection::Summary;
  _defaultDetailName = "Summary";

#ifdef ACT_USE_ROOT
  _defaultDetail = ActOutputSelection::All;
  _defaultDetailName = "All";
#endif

}

std::string ActOutputSelection::setExtension(int type) {

  std::string extension(".out");
  if (type == ActOutputSelection::ROOT) {
    extension = ".root";
  }

  return extension;
}
