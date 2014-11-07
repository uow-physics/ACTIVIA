// Class to represent nuclides; halflife is in days.

#ifndef ACT_NUCLIDE_HH
#define ACT_NUCLIDE_HH

/// \brief Class to represent a nuclear isotope. Half-life is in days.
///
/// Both product and target isotopes are represented by such objects.

class ActNuclide {

 public:

  /// Construct a nuclide with given atomic and mass numbers and half life (in days).
  ActNuclide(int Z, double A, double halfLife = 0.0);
  virtual ~ActNuclide();

  /// The less than operator. This compares Z, then A, then halfLife values.
  bool operator < (const ActNuclide& other) const;
  /// The equality operator. An object is only equal to "other" if Z, A, and halfLife all agree.
  bool operator == (const ActNuclide& other) const;

  void print();

  /// Retrieve the integer atomic number
  int getZ() {return _Z;}
  /// Return the atomic number as a double
  double getfZ() {return _fZ;}
  /// Return the atomic mass number
  double getA() {return _A;}
  /// Get the half life of the nuclide (unit is days)
  double getHalfLife() {return _halfLife;}

 protected:

  int _Z;
  double _fZ, _A, _halfLife;

 private:

};

#endif
