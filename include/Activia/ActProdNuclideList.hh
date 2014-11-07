// Class to build a list of possible product radionuclides from an 
// input file using STL vectors

#ifndef ACT_PROD_NUCLIDE_LIST_HH
#define ACT_PROD_NUCLIDE_LIST_HH

#include "Activia/ActNuclide.hh"
#include "Activia/ActProdNuclide.hh"

#include <vector>
#include <string>

/// \brief Store a list of available product nuclides.
///
/// This class stores the pointers to all available product nuclides that 
/// can be used for cross-section and yield calculations.
/// The nuclides can be specified by an input file or by individual nuclide objects.

class ActProdNuclideList {

 public:

  ActProdNuclideList();
  virtual ~ActProdNuclideList();

  /// Store the list (vector) of product nuclides, including any side-branches
  /// reading in a data table from inFileName. Each row of this table needs to be:
  /// Z A halfLife nSB Z_SB1 A_SB1 Z_SB2 A_SB2 ... Z_SBn A_SBn
  /// where Z, A and halfLife specify the atomic, mass numbers and half-life (days)
  /// of the product isotope, nSB specifies the number of side branch nuclei and
  /// Z_SBi and A_SBi are the atomic and mass numbers for each side branch i (if nSB > 0).
  /// By default the input file is "decayData.dat".
  void storeTable(const char* inFileName = "decayData.dat");
  
  /// Add a product nuclide to the internal list
  void addProdNuclide(ActProdNuclide& prodNuclide);
  /// Add a product nuclide to the internal list
  void addProdNuclide(int Z, double A, double halfLife);

  /// Get the ith product nuclide 
  ActProdNuclide* getProdNuclide(int storeInt);
  /// Get the product nuclide for the given Z and A values
  ActProdNuclide* getProdNuclide(int Z, double A);

  void cleanUp();
  /// Set the debug output flag
  void setDebugFlag(int flag) {_debug = flag;}

  /// Get the number of product nuclides stored in the internal vector
  inline int getNProdNuclides() {return _nProdNuclides;}
  /// Get the debug output flag value
  inline int getDebugFlag() {return _debug;}

  /// Get the name of the file last used to store information via storeTable(fileName)
  std::string getInFileName() {return _inFileName;}

 protected:

 private:

  std::vector<ActProdNuclide*> _prodNuclides;
  int _nProdNuclides, _debug;
  std::string _inFileName;

};

#endif
