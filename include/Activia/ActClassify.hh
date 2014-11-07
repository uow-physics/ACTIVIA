#ifndef ACT_CLASSIFY_HH
#define ACT_CLASSIFY_HH

#include <vector>

/// \brief Class for classifying even-odd pairings for Z,N numbers.

class ActClassify {

 public:

  ActClassify(int ia, int iz);
  virtual ~ActClassify();

  /// Possible pairings between even and odd number of nucleons
  enum pairing {EvenEven, EvenOdd, OddEven, OddOdd};

  /// Return the classifying integer, based on the pairing enum
  /// and the input values of A and Z provided in the constructor
  int classif();

 protected:

 private:

  int _ia, _iz;

};

#endif
