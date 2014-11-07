// Abstract class for selecting algorithms for isotope decay

#include "Activia/ActAbsDecayAlgorithm.hh"

#include "Activia/ActTarget.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActTime.hh"
#include "Activia/ActAbsOutput.hh"

ActAbsDecayAlgorithm::ActAbsDecayAlgorithm(ActTarget* target, ActProdNuclideList* prodList,
					   ActTime* times)
{
  // Constructor
  _theTarget = target;
  _theProdList = prodList;
  _times = times; _decayMap.clear();
  _outputData = 0;
}

ActAbsDecayAlgorithm::~ActAbsDecayAlgorithm() 
{
  // Destructor
  _decayMap.clear();
}
