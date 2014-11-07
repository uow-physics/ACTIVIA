// Class to run the Activia code within a terminal window

#include "Activia/ActXTermRun.hh"
#include "Activia/ActInput.hh"

ActXTermRun::ActXTermRun() : ActAbsRun() {
  // Constructor
}

ActXTermRun::~ActXTermRun() {
  // Destructor
}

void ActXTermRun::defineInput() {

  _input = new ActInput(_outputSelection);

}

