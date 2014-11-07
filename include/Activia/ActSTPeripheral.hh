// Class to calculate the peripheral reactions acc'd to Silberberg and Tsao, 
// AP J Supp 220(I) 335 (1973) + [Sil77], [Sil85], [Sil90]
// Cross section given by sig=sig(ezero)*H(E)*Y(At,Zt)     

#ifndef ACT_ST_PERIPHERAL_HH
#define ACT_ST_PERIPHERAL_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;
class ActSTSigUpdates;

/// \brief Calculate the peripheral reactions (ypxn) using Silberberg-Tsao formulae
///
/// This class calculates the cross-section for region "P", from pg 356 onwards, ST'73 II

class ActSTPeripheral : public ActAbsXSecModel {

 public:

  /// Construct the peripheral model for region P, applying any updates.
  ActSTPeripheral(std::string name, bool applyUpdates, int debug);
  virtual ~ActSTPeripheral();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  // internal functions

  /// Eq 2a in ST'77
  double pxnsig(double x, double nzt);

  /// Eq 3c in ST'77
  double pxng(double at, double e, double x);

  /// Eq 3d in ST'77
  double pxnh(double at, double zt, double ap, 
	      double zp, double e, double x);

  /// Eq 3b in ST'77
  double pxnf(double at, double zt, double ap,
	      double zp, double e, double x);

  /// Eq 23 in ST'73 II
  double yield(double d, double at, double atbar,
	       double atmin, double zt);

  /// Eq 29a in ST'73 II
  double h1(double at, double a, double e);

  /// Eq 29b in ST'73 II
  double h2(double at, double a, double e);

  /// Eq 29c in ST'73 II
  double h3(double at, double a, double e);

  /// Eq 29d in ST'73 II
  double h4(double e);

  /// Decay energies of product nuclei for light targets, Table 1 ST'77
  double ed(double zp, double ap);

  /// Get the name of the light peripheral model
  std::string getLiteName(int iz, int ia, int izt);

  /// Calculate the spallation (light product) cross-section.
  double calcSpalLiteSigma(ActNucleiData* data);

  /// Calculate the cross-section of (p, xn) reactions
  double calcxnSigma(ActNucleiData* data);

  /// Calculate the cross-section of (p, pxn) reactions
  double calcpxnSigma(ActNucleiData* data);

  /// Calculate the cross-section of (p, 2p) reactions
  double calc2pSigma(ActNucleiData* data);

  /// Calculate the cross-section of (p, 2pxn) reactions
  double calc2pxnSigma(ActNucleiData* data);

  /// Calculate the cross-section of (p, 3pxn) reactions
  double calc3pxnSigma(ActNucleiData* data);

  /// Apply corrections to the (p, 3pxn) cross-section (ST'85, Eq 15-18)
  void apply3pxnCorrection(ActNucleiData* data, double& sigma);

  /// Apply general corrections fo the peripheral cross-section
  void applySigmaCorrections(ActNucleiData* data);

  ActSTSigUpdates* _updates;
  bool _applyUpdates;

};

#endif
