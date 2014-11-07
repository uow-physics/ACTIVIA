#ifndef ACT_NUCLIDE_KEY_HH
#define ACT_NUCLIDE_KEY_HH

/// \brief A class used to define a key for use in ActNuclideFactory.
///
/// This key is used to retrieve a given isotope pointer in ActNuclideFactory.
/// Since two isotopes with the same Z, A could have different energy states,
/// and therefore different decay times, we need a key to also compare halfLife
/// values in addition to Z and A values when asking if the isotope already
/// exists in the ActNuclideFactory.

class ActNuclideKey {

 public:

  /// Construct a nuclide key given atomic and mass numbers and the half life.
  ActNuclideKey(int Z, double A, double halfLife);
  virtual ~ActNuclideKey();

  /// Less than operator used for storing and retrieving nuclide objects in
  /// the STL map in ActNuclideFactory.
  bool operator < (const ActNuclideKey& other) const;

  /// Return the integer atomic number.
  inline int getZ() {return _Z;}
  /// Return the mass number.
  inline double getA() {return _A;}
  /// Return the half-life (in days).
  inline double getHalfLife() {return _halfLife;}

 protected:

 private:

  int _Z;
  double _A, _halfLife;

};

#endif
