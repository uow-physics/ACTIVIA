//  Class to calculate cross-sections (mb) for Z<29 targets

#ifndef ACT_ST_SLITE_HH
#define ACT_ST_SLITE_HH

#include "Activia/ActAbsXSecModel.hh"
#include "Activia/ActNucleiData.hh"
#include <vector>
#include <string>

class ActSTSigUpdates;

/// \brief Calculate the cross-setions for Z < 29 targets using Silberberg-Tsao formulae
///
/// This class calculates the cross-section using formulae in ST'73 I, Tables 1A-1D.

class ActSTSLite : public ActAbsXSecModel {

 public:

  /// Construct the cross-section model for Z < 29 targets, applying any updates
  ActSTSLite(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTSLite();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  /// Calculate the cross-section given the target region name
  double calcSLiteSigma(std::string name);

  /// Set up the cross-section calculation parameters.
  void setUp(ActNucleiData& data);

  /// Initialise parameters
  void initLiteParam();

  /// Calculate the cross-section in Table IA, ST'73 I
  void calcLite1Values(int ioe);

  /// Calculate the cross-section in Table IB, ST'73 I
  void calcLite2Values(int ioe);

  /// Calculate the cross-section in Table IC, ST'73 I
  void calcLite3Values(int ioe);

  /// Calculate the cross-section in Table ID, ST'73 I
  void calcLite4Values(int ioe);

  double _zt, _at, _atbar, _z, _a, _e, _ezero;
  int _izt, _iz, _ia, _iat;
  double _logat, _loge;

  //double _azero, _xmp;
  double _t, _s, _p, _r, _nu, _etasel, _sig0, _corr;

  std::vector< std::vector<double> > _eta;

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
