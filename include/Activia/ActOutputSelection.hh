#ifndef ACT_OUTPUT_SELECTION_HH
#define ACT_OUTPUT_SELECTION_HH

#include "Activia/ActAbsData.hh"
#include <string>
#include <vector>
#include <map>

/// \brief Class to define xSec and yield output files, including format and level of detail

class ActOutputSelection {

 public:

  /// Construct output selection class
  ActOutputSelection();
  virtual ~ActOutputSelection();

  /// Allowed output types
  enum OutputTypes {Stream = 0, ROOT};
  /// Level of detail
  enum LevelOfDetail {Summary = 0, All};

  /// Methods to set the output filenames, types and level of detail

  /// Set the cross-section file name
  void setXSecFileName(std::string fileName) {_xSecFileName = fileName;}
  /// Set the decay yield file name
  void setDecayFileName(std::string fileName) {_decayFileName = fileName;}

  /// Set the cross-section level of detail
  void setXSecDetail(int flag);
  /// Set the decay yield level of detail
  void setDecayDetail(int flag);

  /// Set the cross-section output type.
  void setXSecType(int type);

  /// Set the decay yield output type.
  void setDecayType(int type);
  
  /// Set the file name extension depending on the output format type
  std::string setExtension(int type);

  /// Get the output information

  /// Retrieve the cross-section file name
  std::string getXSecFileName() {return _xSecFileName;}
  /// Retrieve the decay yield file name
  std::string getDecayFileName() {return _decayFileName;}

  /// Retrieve the cross-section output type
  int getXSecType() {return _xSecType;}
  /// Retrieve the decay yield output type
  int getDecayType() {return _decayType;}

  /// Retrieve the cross-section output level of detail
  int getXSecDetail() {return _xSecDetail;}
  /// Retrieve the decay yield output level of detail
  int getDecayDetail() {return _decayDetail;}

  /// Get the default output type (integer)
  int getDefaultType() {return _defaultType;}

  /// Get the default output type (name)
  std::string getDefaultTypeName() {return _defaultTypeName;}

  /// Get the default level of detail
  int getDefaultDetail() {return _defaultDetail;}

  /// Get the xSec output file extension
  std::string getXSecExtension() {return _xSecExtension;}

  /// Get the decay output file extension
  std::string getDecayExtension() {return _decayExtension;}

  typedef std::map<int, std::string> ActOutputMap;

  /// Get the allowed output types (integer flags and names)
  ActOutputMap getAllowedTypes() {return _allowedTypes;}

  /// Get the allowed level of detail for the output
  ActOutputMap getAllowedDetails() {return _allowedDetails;}

 protected:

 private:

  std::string _xSecFileName, _decayFileName;
  int _xSecType, _decayType;
  int _xSecDetail, _decayDetail;
  std::string _xSecExtension, _decayExtension;

  /// Specify which output file types are allowed
  void allowedTypes();

  /// Specify which level of details are available
  void allowedDetails();

  ActOutputMap _allowedTypes, _allowedDetails;

  int _defaultType;
  std::string _defaultTypeName;
  int _defaultDetail;
  std::string _defaultDetailName;

};

#endif
