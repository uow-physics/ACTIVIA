// Read and interpolate the cross-section from tables of sigma vs energy
// for target-product pairs (e.g from MENDL-2 data tables).

#ifndef ACT_XSEC_DATA_MODEL_HH
#define ACT_XSEC_DATA_MODEL_HH

#include "Activia/ActAbsXSecModel.hh"
#include "Activia/ActNuclide.hh"
#include "Activia/ActXSecGraph.hh"

#include <string>
#include <vector>
#include <map>

class ActNucleiData;
class ActTargetNuclide;
class ActProdNuclideList;

/// \brief Less than operator between two ActNuclide pointers for ActXSecDataModel.

struct ActDataPtrLess {
  bool operator() (const ActNuclide* ptr1, const ActNuclide* ptr2) const {
    return (*ptr1) < (*ptr2);
  }
};

/// \brief Use data tables to calculate cross-sections.
///
/// This class allows us to use experimental data to obtain cross-sections
/// for target-product isotope pairs. Each individual table must be for
/// only one target isotope but can include any number of product isotopes.
/// The files must be written in a common format to be interpreted and used correctly.
/// The start of the file can contain comment lines each preceeded by the hash (\#) symbol.\n
/// 1. The target isotope is specified by a line\n
///   Target Zt At\n
/// where Zt and At are the atomic and mass numbers of the isotope.\n
/// 2. Next another comment line should be written - this specifies the start of a table for a
/// product isotope, which itself must be defined on the following line\n
///   \# New table comment\n
///   Product Z A\n
/// 3. The following lines must then contain the values of energy (MeV) and cross-section (mb)\n
///   10.0  0.1\n
///   20.0  0.25\n
///   ...   ...\n
/// A new product table can be written after the proceeding table by repeating steps 2 and 3 above.

class ActXSecDataModel : public ActAbsXSecModel {

 public:

  /// Construct the data table model with the given name, inputList of data files and debug flag.
  /// The inputList must contain the file names of the all cross-section data tables.
  ActXSecDataModel(std::string name, std::string inputList, int debug);
  virtual ~ActXSecDataModel();

  /// Calculate the cross-section for a given set of target-product nuclei data.
  virtual double calcCrossSection(ActNucleiData* data);

  /// Store the list of the file names containing the cross-section data tables.
  void initialise();

  /// Load in memory the data tables for the given target isotope and product isotope list.
  void loadDataTables(ActTargetNuclide* targetNuclide, ActProdNuclideList* prodList);

  /// Reads in the file containing the table data for a given target isotope and product isotope list.
  /// 
  void storeXSecGraphs(std::string fileName, 
		       ActNuclide* targetNuclide,
		       ActProdNuclideList* prodList);

  ActXSecGraph getXSecGraph(ActNucleiData* data);

  typedef std::map<ActNuclide*, ActXSecGraph, ActDataPtrLess> ActDataModelMap;
  typedef std::map<ActNuclide*, ActDataModelMap> ActXSecDMAllMap;

 protected:

 private:

  std::string _inputList;
  std::vector<std::string> _listOfDataFiles;

  ActXSecDMAllMap _dataTables;
  ActXSecGraph _theGraph, _nullGraph;

  int _Zt, _Z;
  double _At, _A;

};

#endif
