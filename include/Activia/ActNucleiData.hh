// Class to hold data values for target and product nuclei and related
// quantities that will be useful for cross-section calculations.

#ifndef ACT_NUCLEI_DATA_HH
#define ACT_NUCLEI_DATA_HH

#include "Activia/ActAbsData.hh"

class ActNuclide;
class ActTargetNuclide;

/// \brief Class to hold data values for target and product nuclei.
///
/// Z,A,N values as well as energy and target isotope fractions are stored here.
/// This class is used to quickly pass the relevent numbers to the 
/// cross-section calculation models/formulae.

class ActNucleiData : public ActAbsData {

 public:

  /// Construct nuclei data object given target, product isotope and energy
  ActNucleiData(ActNuclide* beamNuclide, ActTargetNuclide* targetIsotope, 
		ActNuclide* product, double energy);
  ActNucleiData();

  virtual ~ActNucleiData();

  void initialise();

  /// Set the beam composition (Z, A values)
  void setBeamData(ActNuclide* beamNuclide);
  /// Set the target values
  void setTargetData(ActTargetNuclide* targetIsotope);
  /// Set the product isotope values
  void setProductData(ActNuclide* product);

  /// Set the energy of the reaction
  void setEnergy(double e) {_e = e;}

  /// Set the target isotope fraction (between 0 and 1)
  void setFraction(double frac) {_fraction = frac;}
  
  /// Set the threshold energy for reactions (sigma = 0 for E < E_threshold)
  void setThresholdEnergy();
  /// Set the value of E_0, which is where the cross-sections start to become
  /// energy independent
  void setEZero();
  /// Set any other quantities based on the already available values from the target and product.
  void setOtherQuantities();
  /// Calculate the neutron difference between the target and product isotopes: (A_t - Z_t) - (A - Z)
  void calcX();
  /// Calculate the atomic number difference between the target and product isotopes: (Z_t - Z + 1)
  void calcY();

  /// Set the x and y values.
  /// This is used in the ActSTPeripheral class to select what ypxn reaction to calculate.
  void setXandY(double x, double y);
  
  /// Set an integer for ypxn calculations.
  void setichg(int ichg) {_ichg = ichg;}

  /// Get the target atomic number
  inline double getzt() {return _zt;}
  /// Get the target isotope mass number
  inline double getat() {return _at;}
  /// Get the atomic mass "centre-of-gravity" of the target isotopes (see p320 ST'73 I).
  inline double getatbar() {return _atbar;}
  /// Get the product isotope atomic number
  inline double getz() {return _z;}
  /// Get the product isotope mass number
  inline double geta() {return _a;}
  /// Get the energy of the reaction
  inline double gete() {return _e;}
  /// Get the value of E_0, above which cross-sections are independent of energy
  inline double getezero() {return _ezero;}
  /// Get the beam atomic number
  inline double getzbeam() {return _zbeam;}
  /// Get the beam atomic mass
  inline double getabeam() {return _abeam;}

  /// Get the number of target neutrons
  inline double getnt() {return _nt;}
  /// Get the number of product isotope neutrons
  inline double getn() {return _n;}
  /// Get the difference between the number of neutrons and protons in the target isotope: N_t - Z_t
  inline double getdnpt() {return _dnpt;}
  /// Get the difference between the number of neutrons and protons in the product isotope: N - Z
  inline double getdnp() {return _dnp;}

  /// Get the integer target atomic number
  inline int getizt() {return _izt;}
  /// Get the integer target mass number
  inline int getiat() {return _iat;}
  /// Get the integer product isotope atomic number
  inline int getiz() {return _iz;}
  /// Get the integer product isotope mass number
  inline int getia() {return _ia;}
  /// Get the integer beam atomic number
  inline int getizbeam() {return _izbeam;}
  /// Get the integer beam mass number
  inline int getiabeam() {return _iabeam;}

  /// Get the integer target neutron number
  inline int getint() {return _int;}
  /// Get the integer product isotope neutron number
  inline int getin() {return _in;}
  /// Get the integer number for N_t - Z_t for the target isotope
  inline int getidnpt() {return _idnpt;}
  /// Get the integer number for N - Z for the product isotope
  inline int getidnp() {return _idnp;}

  /// Get the value of x: (A_t - Z_t) - (A - Z)
  inline double getx() {return _x;}
  /// Get the value of y: (Z_t - Z + 1)
  inline double gety() {return _y;}
  /// Get the integer value of x
  inline int getix() {return _ix;}
  /// Get the integer value of y
  inline int getiy() {return _iy;}
  inline int getichg() {return _ichg;}

  /// Get the deltaM for the target
  inline double getTargetDeltaM() {return _mdtgt;}
  /// Get the threshold energy for the cross-section calculations
  inline double getThresholdEnergy() {return _thrse;}
  /// Get the target isotope fraction (between 0 and 1).
  inline double getFraction() {return _fraction;}

  /// Calculate an integer for ypxn reactions.
  int calcichg(int ix, int iy);

  void print();

  /// Return the target isotope pointer
  ActTargetNuclide* getTargetIsotope() {return _targetIsotope;}
  /// Return the product isotope pointer
  ActNuclide* getProduct() {return _product;}

 protected:

 private:

  ActNuclide* _beamNuclide;
  ActTargetNuclide* _targetIsotope;
  ActNuclide* _product;

  double _zbeam, _abeam;
  int _izbeam, _iabeam;

  double _zt, _at, _atbar, _fraction;
  double _z, _a, _e, _ezero;
  int _izt, _iat, _iz, _ia;

  double _nt, _n, _dnpt, _dnp;
  int _int, _in, _idnpt, _idnp;

  double _x, _y;
  int _ix, _iy, _ichg;

  double _mdtgt, _thrse;

};

#endif
