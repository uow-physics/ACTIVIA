#ifndef ACT_ABS_CALC_STATUS_HH
#define ACT_ABS_CALC_STATUS_HH

/// \brief Keep track of what has been calculated (cross-sections, yields)

class ActAbsCalcStatus {

public:

  /// Default constructor
  ActAbsCalcStatus();

  /// Destructor
  virtual ~ActAbsCalcStatus();

  /// Set the run status
  void setRunCode(bool status) {_runCode = status;}

  /// Retrieve the run status (default is true)
  bool canRunCode() {return _runCode;}

  /// Set the number of isotopes that target has
  void setNTargetIsotopes(int nIsotopes);
  /// Set which target isotope we are working on
  void setTargetIsotope(int iIsotope);

  /// Get the total number of isotopes in the target
  int getNTargetIsotopes() {return _nTIsotopes;}
  /// Get which isotope number we are working on
  int getTargetIsotope() {return _iTIsotope;}

  /// Set the number of product isotopes
  void setNProductIsotopes(int nIsotopes);
  /// Set which product isotope is being worked on
  void setProductIsotope(int iIsotope);

  /// Get the total number of product isotopes
  int getNProductIsotopes() {return _nPIsotopes;}
  /// Get which product isotope is being worked on
  int getProductIsotope() {return _iPIsotope;}

  /// Report on cross-section calculation status
  virtual void xSecReport() = 0;
  /// Report on decay yield calculation status
  virtual void decayReport() = 0;

  /// Any set-up
  virtual void setUp() {;}

  /// Finalise
  virtual void finalise() {;}

  /// Reset
  virtual void reset() {;}

protected:
  
  bool _runCode;
  int _nTIsotopes, _iTIsotope, _nPIsotopes, _iPIsotope;

private:

};

#endif
