// Calculate fission cross sections: use equations 2a for sigmas,  
// 4a for fofa, 5 for ffofe, 38a,b for phi(at,e), plug all into eqn 6
// for sigma (in Ap J Supp 220,25,335(1973))

#include "Activia/ActSTFissSpallation.hh"
#include "Activia/ActSTSpallation.hh"
#include "Activia/ActSTFission.hh"
#include "Activia/ActNucleiData.hh"
#include "Activia/ActSTSigUpdates.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActSTFissSpallation::ActSTFissSpallation(std::string name, bool applyUpdates, int debug) : ActAbsXSecModel(name, debug)
{
  // Constructor
  _updates = new ActSTSigUpdates();
  _applyUpdates = applyUpdates;
}

ActSTFissSpallation::~ActSTFissSpallation() 
{
  // Destructor
  delete _updates; _updates = 0;
}

double ActSTFissSpallation::calcCrossSection(ActNucleiData* data) {

  if (data == 0) {
    cout<<"Error in ActSTFissSpallation. Data object is null"<<endl;
    return 0.0;
  }

  _sigma = 0.0;

  ActSTSpallation spallation("spal", false, _debug);
  _sigma = spallation.calcCrossSection(data);

  
  ActSTFission fission("fiss", false, _debug);
  double sigmaf = fission.calcCrossSection(data);
  _sigma = fmax(_sigma, sigmaf);

  if (_applyUpdates == true) {_updates->updateSigma(*data, _sigma);}

  return _sigma;

}

