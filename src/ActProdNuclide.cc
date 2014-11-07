// Class to represent product nuclides, with any side branch nuclides

#include "Activia/ActProdNuclide.hh"
#include "Activia/ActNuclideFactory.hh"

#include <iostream>

using std::cout;
using std::endl;

ActProdNuclide::ActProdNuclide(int Z, double A, double halfLife) : ActNuclide(Z, A, halfLife)
{
  // Constructor
  _sbNuclei.clear(); _nSideBranches = 0;
  _product = ActNuclideFactory::getInstance()->getNuclide(Z, A, halfLife);
}

ActProdNuclide::~ActProdNuclide() 
{
  // Destructor
  _sbNuclei.clear();
}

void ActProdNuclide::addSideBranch(int Z, double A) {

  //cout<<"Adding side branch Z = "<<Z<<", A = "<<A<<" to the product nuclide Z = "
  //<<_Z<<" and A = "<<_A<<endl;

  double halfLife(0.0); // short-lived side branch nuclide

  ActNuclide* sideBranch = ActNuclideFactory::getInstance()->getNuclide(Z, A, halfLife);
  _sbNuclei.push_back(sideBranch);

  _nSideBranches = (int) _sbNuclei.size();

}

void ActProdNuclide::addSideBranch(ActNuclide* sideBranch) {

  if (sideBranch != 0) {

    //cout<<"Adding side branch Z = "<<sideBranch->getZ()
    //<<", A = "<<sideBranch->getA()<<" to the product nuclide Z = "
    //<<_Z<<" and A = "<<_A<<endl;

    _sbNuclei.push_back(sideBranch);

    _nSideBranches = (int) _sbNuclei.size();
  }

}

ActNuclide* ActProdNuclide::getSideBranch(int order) {

  if (order < 0 || order >= _nSideBranches) {
    cout<<"Error in getSBNuclide: order = "<<order
	<<" is outside the range 0 to "<<_nSideBranches-1<<endl;
    return 0;
  }

  ActNuclide* nuclide = _sbNuclei[order];

  return nuclide;

}

void ActProdNuclide::print() {

  cout<<"ProdNuclide (Z,A) = ("<<_Z<<", "<<_A
      <<"), halfLife = "<<_halfLife
      <<", number of side branch nuclides = "<<_nSideBranches<<endl;

  int i;
  for (i = 0; i < _nSideBranches; i++) {

    ActNuclide* nuclide = _sbNuclei[i];
    if (nuclide != 0) {
      cout<<"Side branch nuclide "<<i<<" has (Z,A) = ("<<nuclide->getZ()
	  <<", "<<nuclide->getA()<<")"<<endl;
    }    
  }

}
