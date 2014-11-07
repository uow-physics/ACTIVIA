#ifndef ACT_TRITIUM_MODEL_HH
#define ACT_TRITIUM_MODEL_HH

#include "Activia/ActAbsXSecModel.hh"
#include <string>

class ActNucleiData;

/// \brief Calculate the tritium cross-section.
///
/// This uses formulae in A.Yu. Konobeyev and Yu.A. Korovin, NIM B82 (1993) 103-115.

class ActTritiumModel : public ActAbsXSecModel {

 public:

  /// Construct the tritium cross-section calculation model.
  ActTritiumModel(std::string name, int debug);
  virtual ~ActTritiumModel();

  /// Calculate the cross-section given the target and product isotope data.
  virtual double calcCrossSection(ActNucleiData* data);

 protected:

 private:

  double sigmanon(ActNucleiData* data);

};

#endif
