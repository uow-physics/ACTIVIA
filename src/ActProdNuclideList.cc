// Class to build a list of possible product radionuclides from an 
// input file using STL vectors

#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActNuclideFactory.hh"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>

using std::cout;
using std::endl;

ActProdNuclideList::ActProdNuclideList()
{
  // Constructor
  _prodNuclides.clear(); _nProdNuclides = 0; _debug = 0;
  _inFileName = "";
}

ActProdNuclideList::~ActProdNuclideList() 
{
  // Destructor
  this->cleanUp();
}

void ActProdNuclideList::cleanUp() {

  int i;
  for (i = 0; i < _nProdNuclides; i++) {
    ActProdNuclide* prodNuclide = _prodNuclides[i];
    delete prodNuclide; prodNuclide = 0;
  }
  _prodNuclides.clear();
}

void ActProdNuclideList::storeTable(const char* inFileName) {

  cout<<"Storing radionuclide data using input file "<<inFileName<<endl;

  _inFileName = std::string(inFileName);
  std::ifstream getData(inFileName);

  // Check if the file exists. Hopefully this will be done by the appropriate input
  // class, but check again here just in case.
  if (!getData.is_open()) {

    cout<<"\nError in ActProdNuclideList::storeTable."<<endl;
    cout<<"The file "<<inFileName<<" does not exist."<<endl;
    cout<<"Table containing nuclides (A, Z, halfLife and side branch info) is not stored."<<endl;
    cout<<"The program will not calculate anything useful. ";
    cout<<"Please fix the nuclide input data file."<<endl;
    getData.close();
    // This is a major configuration error, and the program needs to be stopped.
    exit(-1);
  }

  // Read each line in the input file and store the nuclide data
  int i, nSB, Z;
  double A, halfLife;

  this->cleanUp();

  while (getData.good()) {

    getData >> Z >> A >> halfLife >> nSB;

    // Stop if we have reached the end of the file.
    if (getData.eof()) {break;}

    ActProdNuclide* prodNuclide = new ActProdNuclide(Z, A, halfLife);

    for (i = 0; i < nSB; i++) {

      int sbZ;
      double sbA;
      getData >> sbZ >> sbA;

      prodNuclide->addSideBranch(sbZ, sbA);

    }

    // Store the data and put into the vector.
    _prodNuclides.push_back(prodNuclide);

  }

  _nProdNuclides = (int) _prodNuclides.size();

  cout<<"Finished storing "<<_nProdNuclides<<" product radio-nuclides"<<endl;

}

void ActProdNuclideList::addProdNuclide(ActProdNuclide& prodNuclide) {

  ActProdNuclide* newNuclide = new ActProdNuclide(prodNuclide);
  _prodNuclides.push_back(newNuclide);
  _nProdNuclides = (int) _prodNuclides.size();

}

void ActProdNuclideList::addProdNuclide(int Z, double A, double halfLife) {

  ActProdNuclide* newNuclide = new ActProdNuclide(Z, A, halfLife);  
  _prodNuclides.push_back(newNuclide);
  _nProdNuclides = (int) _prodNuclides.size();

}

ActProdNuclide* ActProdNuclideList::getProdNuclide(int storeInt) {

  if (storeInt >= _nProdNuclides || storeInt < 0) {
    cout<<"Error in ActProdNuclideList::getProdNuclide. "
	<<"storeInt = "<<storeInt<<" must be between 0 and "
	<<_nProdNuclides-1<<endl;
    return 0;
  }

  ActProdNuclide* prodNuclide = _prodNuclides[storeInt];
  return prodNuclide;

}

ActProdNuclide* ActProdNuclideList::getProdNuclide(int Z, double A) {

  ActProdNuclide* prodNuclide(0);
  bool gotNuclide(false);
  int nLoop(0);

  // Loop over the available nuclides and find the match
  // for the given Z and A value
  while (gotNuclide == false && nLoop < _nProdNuclides) {

    ActProdNuclide* isoEntry = _prodNuclides[nLoop];
    int isoZ = isoEntry->getZ();
    double isoA = isoEntry->getA();

    if ((Z == isoZ) && std::fabs(A - isoA) < 1e-6) {
      gotNuclide = true;
      prodNuclide = isoEntry;
    }

    nLoop++;

  }

  if (gotNuclide == false && _debug == 1) {
    cout<<"Error in ActProdNuclideList::getProdNuclide(int Z, double A)."<<endl;
    cout<<"Couldn't find nuclide for Z = "<<Z<<" and A = "<<A<<endl;
  }

  return prodNuclide;
}
