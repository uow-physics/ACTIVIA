// Class for storing a list of all possible ActNuclide objects that
// the code may use

#include "Activia/ActNuclideFactory.hh"
#include "Activia/ActNuclide.hh"

ActNuclideFactory::ActNuclideFactory()
{
  // Constructor
  _map.clear();
}

ActNuclideFactory::~ActNuclideFactory() 
{
  // Destructor
  ActFactoryMap::iterator iter;
  for (iter = _map.begin(); iter != _map.end(); ++iter) {
    ActNuclide* nuclide = iter->second;
    delete nuclide;
  }
  _map.clear();
}

ActNuclideFactory* ActNuclideFactory::getInstance() {

  static ActNuclideFactory* theFactory = 0;
  if (theFactory == 0) {
    theFactory = new ActNuclideFactory();
  }
  return theFactory;

}

ActNuclide* ActNuclideFactory::getNuclide(int Z, double A, double halfLife) {

  // There could be two isotopes with the same Z,A (different energy states).
  // Therefore need to use a key object to also distinguish between these cases.
  ActNuclideKey key(Z, A, halfLife);
  ActFactoryMap::iterator iter;

  ActNuclide* theNuclide(0);

  if ((iter = _map.find(key)) != _map.end()) {

    theNuclide = iter->second;
    
  } else {

    // The A,Z value is not in the map. Store it now and also return it.
    theNuclide = new ActNuclide(Z, A, halfLife);
    _map[key] = theNuclide;

  }

  return theNuclide;

}
