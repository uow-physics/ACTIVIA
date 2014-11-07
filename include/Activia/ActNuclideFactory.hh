#ifndef ACT_NUCLIDE_FACTORY_HH
#define ACT_NUCLIDE_FACTORY_HH

#include "Activia/ActNuclideKey.hh"

class ActNuclide;

#include <map>

/// \brief A factory class for creating nuclear isotope objects
///
/// This class must be used to create nuclear isotopes. It uses the factory
/// method whereby if the required isotope already exists it returns the pre-existing pointer
/// and does not create a new object. This significantly saves on memory use, and avoids 
/// the need to continually create new isotope objects whenever they are used throughout the code.

class ActNuclideFactory {

 public:

  ActNuclideFactory();
  virtual ~ActNuclideFactory();

  /// Get a static instance of this factory class. Only one is created per application.
  static ActNuclideFactory* getInstance();

  /// Retrieve the nuclide if it already exists. Otherwise it returns a new nuclide pointer,
  /// which is also put inside the internal map. Any subsequent calls using the same nuclide 
  /// data (Z, A, halfLife) will return the pre-existing pointer.
  ActNuclide* getNuclide(int Z, double A, double halfLife);

 protected:

  /// A typedef to define a map used to keep track of which isotopes have been created.
  typedef std::map<ActNuclideKey, ActNuclide*> ActFactoryMap;
  
 private:

  ActFactoryMap _map;

};

#endif
