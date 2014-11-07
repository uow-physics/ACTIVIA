#ifdef ACT_USE_QT

#ifndef ACT_GUI_CALC_STATUS_HH
#define ACT_GUI_CALC_STATUS_HH

#include "Activia/ActAbsCalcStatus.hh"
#include "Activia/ActQtDebugStream.hh"

#include "QtGui/qwidget.h"
#include "QtGui/qtextedit.h"
#include "QtGui/qprogressbar.h"
#include "QtGui/qgroupbox.h"
#include "QtGui/qpushbutton.h"
#include "QtGui/qboxlayout.h"

/// \brief Keep track of what has been calculated (cross-sections, yields) - for GUI progress bar

class ActGuiCalcStatus : public QWidget, public ActAbsCalcStatus {

  Q_OBJECT

public:

  /// Constructor
  ActGuiCalcStatus(QWidget* parent = 0);

  /// Destructor
  virtual ~ActGuiCalcStatus();
  
protected:
  
  /// Report on cross-section calculation status
  virtual void xSecReport();
  /// Report on decay yield calculation status
  virtual void decayReport();

  /// GUI set-up
  virtual void setUp();
  
  /// Finalise
  virtual void finalise();

  /// Reset
  virtual void reset();

private:

  QWidget* _parent;
  QTextEdit* _textEdit;
  QProgressBar* _progressBar;

  QGroupBox* _logBox;
  QPushButton* _clearButton, *_closeButton;
  QVBoxLayout* _logLayout;

  ActQtDebugStream* _qOut; 

  float _xSecFrac, _decayFrac;
  int _xSecTally;

};

#endif

#endif
