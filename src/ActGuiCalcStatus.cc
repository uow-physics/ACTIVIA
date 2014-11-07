#ifdef ACT_USE_QT

// A class to keep track of what has been calculated (cross-sections, yields)
// Use this to update GUI progress bars/cout statements etc..
#include "Activia/ActGuiCalcStatus.hh"

#include "QtCore/qcoreapplication.h"

#include <iostream>

ActGuiCalcStatus::ActGuiCalcStatus(QWidget* parent) : QWidget(parent), ActAbsCalcStatus()
{
  // Constructor
  _parent = parent;
  _textEdit = new QTextEdit();
  _progressBar = new QProgressBar();
  _xSecFrac = 0.85; _decayFrac = 1.0 - _xSecFrac;
  _xSecTally = 0;

  _logBox = new QGroupBox(parent);
  _clearButton = new QPushButton(QObject::tr("Clea&r Log Text"));
  _closeButton = new QPushButton(QObject::tr("&Close Window"));

  _logLayout = new QVBoxLayout();

  _qOut = new ActQtDebugStream(std::cout, _textEdit);
}


ActGuiCalcStatus::~ActGuiCalcStatus() 
{
  // Destructor
  delete _textEdit;
  delete _progressBar;

  delete _qOut;

}

void ActGuiCalcStatus::setUp() {

  _logBox->setWindowTitle(QObject::tr("ACTIVIA Calculation Output"));
  _logBox->setWindowFlags(Qt::Dialog);

  _progressBar->setMinimum(0);
  _progressBar->setValue(0);

  _nTIsotopes = 0; _nPIsotopes = 0;
  _iTIsotope = 0; _iPIsotope = 0;
  _xSecTally = 0;

  _textEdit->setLineWrapMode(QTextEdit::NoWrap);
  _textEdit->setReadOnly(true);
  // Also clear up the text log output
  _textEdit->clear();

  connect(_clearButton, SIGNAL(clicked()), _textEdit, SLOT(clear()));
  connect(_closeButton, SIGNAL(clicked()), _logBox, SLOT(close()));

  // Redirect the output from cout to the text editor widget
  // Log text will only show at the end of the run. Have not
  // found a way to print out the text as the program runs.
  
  // Set the log window layout
  _logLayout->addWidget(_progressBar);
  _logLayout->addWidget(_textEdit);
  _logLayout->addWidget(_clearButton);
  _logLayout->addWidget(_closeButton);
  _logBox->setLayout(_logLayout);
  
  // Draw the layout window
  _logBox->show();
  
}

void ActGuiCalcStatus::xSecReport() {

  // Update the progress bar. This class should know
  // about the total number of target and product isotopes,
  // as well as which target-product pair is being worked on, assuming the 
  // calculation code sets them correctly.
  // Use this information to say how far the calculation has progressed.
  int prodTotal = _nTIsotopes*_nPIsotopes;
  int n = _iTIsotope*_nPIsotopes + _iPIsotope;
  // Apply a fraction to estimate difference between xSec and decay yield calculation times.
  n = (int) (n*_xSecFrac);
  _xSecTally = n;

  _progressBar->setMaximum(prodTotal);
  _progressBar->setValue(n);

  // This should automatically update the output directed to the _textEdit box 
  // using the ActQtDebugStream class.
  QCoreApplication::instance()->processEvents();

}

void ActGuiCalcStatus::decayReport() {

  // Update the progress bar.
  int prodTotal = _nTIsotopes*_nPIsotopes;
  int n = _iTIsotope*_nPIsotopes + _iPIsotope;
  // Apply a fraction to estimate difference between xSec and decay yield calculation times.
  n = (int) (n*_decayFrac);
  n += _xSecTally;
  
  _progressBar->setMaximum(prodTotal);
  _progressBar->setValue(n);
  
  QCoreApplication::instance()->processEvents();

}

void ActGuiCalcStatus::finalise() {

  // Set the progress bar to 100%
  int prodTotal = _nTIsotopes*_nPIsotopes;

  if (prodTotal < 0) {prodTotal = 1;}

  _progressBar->setMaximum(prodTotal);
  _progressBar->setValue(prodTotal);

  if (_runCode == false) {
    std::cout<<"\n\nCalculations were aborted. Output is not useable."<<std::endl;
  }

}

void ActGuiCalcStatus::reset() {

  // Reset the progress bar
  _progressBar->setValue(0);
  _nTIsotopes = 0; _nPIsotopes = 0;
  _iTIsotope = 0; _iPIsotope = 0;
  _xSecTally = 0;

  // Also clear up the text log output
  _textEdit->clear();

}

#endif
