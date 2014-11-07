#ifndef ACT_ST_SPALLATION_HH
#define ACT_ST_SPALLATION_HH

#include "Activia/ActAbsXSecModel.hh"
#include <vector>
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate spallation cross-sections for Z > 28 targets using Silberberg-Tsao formulae
///
/// This class calculates the cross-section for various spallation regions defined in ST'73 II
/// (Z > 28 targets). It uses sigma_0 expressions defined in Table 1, ST'73 I.

class ActSTSpallation : public ActAbsXSecModel {

 public:

  /// Construct the spallation model for various S regions, applying any updates.
  ActSTSpallation(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTSpallation();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

  /// Calculate the spallation cross-section using the given value of deltaA
  double calcSpallSigma(ActNucleiData* data, double& dela);

  /// Calculate the magic number yield curve shift (p338, ST'73 II)
  double calcMValue(std::string name, int izt, int iz);

  /// Calculate the P value from Table 1D, ST'73 I.
  double calcPValue(ActNucleiData* data);

 protected:

 private:

  void setUp();

  std::vector< std::vector<double> > _eta;

  double _EMaxLimit, _E1GeV, _EMaxDiff;

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

  double _etasel;
  

};

#endif
