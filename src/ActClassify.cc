// Class for (Z,N) pairing classification (e.g. even-even)

#include "Activia/ActClassify.hh"

#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

ActClassify::ActClassify(int ia, int iz)
{
  // Constructor
  _ia = ia; _iz = iz;
}

ActClassify::~ActClassify() 
{
  // Destructor
}

int ActClassify::classif() {

  int ioe = 0;
  int halfa = _ia/2;
  int halfz = _iz/2;

  // note that 2*integer/2 != integer, due to rounding off the decimal place
  int newa = halfa*2;
  int newz = halfz*2;

  int diffa = newa - _ia;
  int diffz = newz - _iz;

  if (diffa == 0 && diffz == 0) {
    ioe = ActClassify::EvenEven;
  } else if (diffa != 0 && diffz == 0) {
    ioe = ActClassify::EvenOdd;
  } else if (diffa == 0 && diffz != 0) {
    ioe = ActClassify::OddOdd;
  } else if (diffa != 0 && diffz != 0) {
    ioe = ActClassify::OddEven;
  }

  return ioe;

}
