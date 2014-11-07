#ifndef ACT_TIME_HH
#define ACT_TIME_HH

#include "Activia/ActAbsData.hh"

/// \brief Simple class to define the beam exposure and decay times (days).

class ActTime : public ActAbsData {

 public:

  /// Construct time object storing the beam exposure and decay times.
  ActTime(double exposureTime, double decayTime);
  virtual ~ActTime();

  /// Get the beam exposure time (days)
  double getExposureTime() {return _exposureTime;}
  /// Get the decay time after the target is removed from the input beam (days).
  double getDecayTime() {return _decayTime;}

 protected:

 private:

  double _exposureTime, _decayTime;

};

#endif
