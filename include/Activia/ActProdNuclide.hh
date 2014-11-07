// Class to represent product nuclides, with any side branch nuclides

#ifndef ACT_PROD_NUCLIDE_HH
#define ACT_PROD_NUCLIDE_HH

#include "Activia/ActNuclide.hh"

#include <vector>

/// \brief Define a product nuclide, with any side branches.
///
/// This class constructs a product isotope which also knows about any
/// short-lived (halfLife < 1 day) side-branch nuclei.
/// Any side-branch nuclei can (should) be included in the cross-section and yield
/// calculations. For example, we can have the following branch decays:
/// Be-14 -> B-14 + e- + anti_nu_e (tHalf = 4 ms), B-14 -> C-14 + e- _ anti_nu_e (tHalf = 14 ms),
/// with C-14 having a half-life of 5730 years. This means that both Be-14 and B-14
/// can be considered side-branches of C-14, since their decays will add to the overall
/// yield of C-14.

class ActProdNuclide : public ActNuclide {

 public:

  /// Construct a product isotope with given atomic and mass numbers and halfLife (days).
  ActProdNuclide(int Z, double A, double halfLife = 0.0);

  /// Add short-lived (halfLife < 1 day) side-branch isotope
  void addSideBranch(int Z, double A);
  /// Add short-lived (halfLife < 1 day) side-branch isotope
  void addSideBranch(ActNuclide* sideBranch);

  virtual ~ActProdNuclide();

  void print();

  /// Get the total number of side-branches this product isotope has.
  inline int getNSideBranches() {return _nSideBranches;}

  /// Get all of the side-branch nuclei this product isotope has.
  std::vector<ActNuclide*> getSideBranchNuclei() {return _sbNuclei;}
  /// Get a specific side-branch nuclide.
  ActNuclide* getSideBranch(int order);

  /// Return only the product isotope nuclide pointer.
  ActNuclide* getProduct() {return _product;}

 protected:

 private:

  int _nSideBranches;
  std::vector<ActNuclide*> _sbNuclei;
  ActNuclide* _product;

};

#endif
