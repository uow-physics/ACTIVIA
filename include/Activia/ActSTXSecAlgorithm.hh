// Abstract class for selecting algorithms for cross-section calculations

#ifndef ACT_ST_XSEC_ALGORITHM_HH
#define ACT_ST_XSEC_ALGORITHM_HH

#include "Activia/ActAbsXSecAlgorithm.hh"

#include <string>

class ActAbsXSecModel;
class ActNucleiData;
class ActTargetNuclide;
class ActProdNuclideList;

class ActTritiumModel;
class ActSTSLite;
class ActSTEvaporation;
class ActSTPeripheral;
class ActSTSpallation;
class ActSTBreakup;
class ActSTFission;
class ActSTFissSpallation;
class ActSTFissBreakup;
class ActSTFissSpallGamma;
class ActXSecDataModel;

/// \brief Class that defines how Silberberg-Tsao cross-sections are calculated.
///
/// The algorithm first finds the cross-section from any pre-loaded data tables.
/// If the data cross-section is below _minDataSigma (0.001 mb by default), 
/// then the Silberberg-Tsao formulae are used instead. This class uses
/// the ActSTSelect method to select the appropriate model, e.g. spallation. 
/// The cross-section is then calculated using the class containing the required 
/// Silberberg-Tsao formulae.

class ActSTXSecAlgorithm : public ActAbsXSecAlgorithm {

 public:

  /// Construct the cross-section algorithm using Silberberg-Tsao models.
  ActSTXSecAlgorithm(const char* listOfDataTables = "", double minDataXSec = 0.0);
  virtual ~ActSTXSecAlgorithm();

  /// Set the target and product isotope data for use in the calculations
  virtual void setNucleiData(ActNucleiData* data);

  /// Calculate the target-product isotope cross-section
  virtual double calcCrossSection();

  /// Load in any data tables of cross-section versus energy
  virtual void loadDataTables(ActTargetNuclide* targetNuclide,
			      ActProdNuclideList* prodList);

  /// Only calculate cross-sections for processes with at least two end products,
  /// i.e. X + p -> Y_1 + Y_2 and not X + p -> Y + gamma.
  virtual bool passSelection(ActNucleiData* data);

  /// Check if the energy is above the threshold energy
  virtual bool passESelection(ActNucleiData* data);

  /// Create the list of Silberberg-Tsao models.
  void createListOfModels();

  /// Select the appropriate Silberber-Tsao model based on the target and product information.
  void selectXSecModel(ActNucleiData* data);

  /// Get the minimum allowed cross-section from data tables.
  double getMinDataSigma() {return _minDataSigma;}

  /// Set the minimum allowed cross-section from data tables.
  /// If the cross-section is below this limit, the ST formulae are used instead.
  void setMinDataSigma(double sigma) {_minDataSigma = sigma;}

 protected:
  
 private:

  ActTritiumModel* _trit;
  ActSTSLite* _lit1, *_lit2, *_lit3, *_lit4, *_lit5;
  ActSTEvaporation* _evap;
  ActSTPeripheral* _prph;
  ActSTSpallation* _spal;
  ActSTBreakup* _brkp;
  ActSTFission* _fiss;
  ActSTFissSpallation* _mxfs;
  ActSTFissBreakup* _mxbf;
  ActSTFissSpallGamma* _fgsg;

  ActXSecDataModel* _dataModel;

  ActAbsXSecModel* _currentModel;

  std::string _listOfDataTables;

  /// Set the minimum allowed value of the cross-section from data tables.
  /// If sigma_data is below this limit, then the Silberberg-Tsao formulae are used instead.
  double _minDataSigma;
  
};

#endif
