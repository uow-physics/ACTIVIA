#ifdef ACT_USE_QT

#ifndef ACT_GUI_WINDOW_HH
#define ACT_GUI_WINDOW_HH

#include "Activia/ActOutputSelection.hh"

#include "QtGui/qabstractbutton.h"
#include "QtGui/qbuttongroup.h"
#include "QtGui/qcombobox.h"
#include "QtGui/qgridlayout.h"
#include "QtGui/qgroupbox.h"
#include "QtGui/qlineedit.h"
#include "QtGui/qmenubar.h"
#include "QtGui/qpushbutton.h"
#include "QtCore/qstring.h"
#include "QtGui/qtablewidget.h"
#include "QtGui/qwidget.h"
#include "QtGui/qspinbox.h"
#include "QtGui/qradiobutton.h"
#include "QtGui/qlabel.h"

/// \brief Define the GUI window for collecting data for the calculations

class ActGuiWindow : public QWidget {

  Q_OBJECT

public:

  /// Default constructor
  ActGuiWindow(QWidget* parent = 0);

  /// Constructor
  ActGuiWindow(QWidget* parent = 0, ActOutputSelection* outputSelection = 0);

  /// Destructor
  virtual ~ActGuiWindow();

  /// Create the various input boxes
  void createBoxes();

  /// Retrieve the input box specifying the target isotopes
  QGroupBox* getTargetBox() {return _targetBox;}
  /// Retrieve the input box specifying the calculation mode
  QGroupBox* getCalcBox() {return _calcBox;}
  /// Retrieve the input box specifying the product nuclide list
  QGroupBox* getNuclideBox() {return _nuclideBox;}
  /// Retrieve the input box specifying the energy beam spectrum
  QGroupBox* getSpectrumBox() {return _spectrumBox;}
  /// Retrieve the input box specifying the data tables
  QGroupBox* getDataTablesBox() {return _dataTablesBox;}
  /// Retrieve the input box specifying the exposure and decay times
  QGroupBox* getTimeBox() {return _timeBox;}
  /// Retrieve the input box specifying the output file names etc..
  QGroupBox* getOutputBox() {return _outputBox;}

  /// A pointer to the table specifying the target isotopes
  QTableWidget* getTargetTable() {return _targetTable;}
  int getTargetZ();

  /// Check if all products are to be calculated
  bool doAll() {return _doAll;}
  /// Check if only one product is to be calculated
  bool doSingle() {return _doSingle;}

  /// Get the name of the isotope input file
  std::string getIsotopeFileName();

  /// Get the atomic number of the single isotope product
  int getSingleZ();
  /// Get the atomic mass of the single isotope product
  double getSingleA();

  /// Get the starting input beam energy
  double getEStart();
  /// Get the end beam energy
  double getEEnd();
  /// Get the beam energy bin width
  double getdE();

  /// Check if we need to use data tables
  bool useDataTableFile() {return _useDataTables;}
  /// Retrieve the minimum cross-section value for the data tables
  double getMinSigma();
  /// Get the name of the file specifying the various input data files to be used
  std::string getDataTablesFile();

  /// Get the beam exposure time (days)
  double getExposureTime();
  /// Get the decay (cooling) time (days)
  double getDecayTime();

  /// Save the GUI form input to the given file
  void saveInput(const QString& fileName);
  /// Load the input from the given file to present the GUI inputs
  void loadInput(const QString& fileName);
  /// Clear all GUI inputs
  void clearInput();

public slots:

  /// Update the target isotope table with the new number of rows
  void updateTargetTable(int nRows);
  /// Specify whether we want to do calculations for all isotope products
  void doAll(bool status);
  /// Specify whether we want to do calculations for only one product isotope
  void doSingle(bool status);

  /// Open a file dialog window to ask for the file specifying the available product isotopes
  void getNuclideList();
  /// Open a file dialog window to ask for the file specifying the list of data table files
  void getDataTables();
  /// Specify if we want to use data tables
  void useDataTables(bool status);

  /// Store the format of the cross-section output file
  void getXSecFormat(QAbstractButton* button);
  /// Store the format of the decay yield output file
  void getDecayFormat(QAbstractButton* button);
  /// Store the level of detail that the cross-section output file will use
  void getXSecDetail(QAbstractButton* button);
  /// Store the level of detail that the decay yield output file will use
  void getDecayDetail(QAbstractButton* button);

  /// Open a file dialog window to ask for the filename that will store the cross-section output
  void getXSecOutFile();
  /// Open a file dialog window to ask for the filename that will store the decay yield output
  void getDecayOutFile();

  /// Set the filename of the cross-section output
  void setXSecOutFileName(const QString& text);
  /// Set the filename of the decay yield output
  void setDecayOutFileName(const QString& text);

protected:

private:

  /// Initialise the window
  void initialise();

  /// Create the box that will store the target isotope information
  void createTargetBox();
  /// Create the box that will store the calculation mode details
  void createCalcMode();
  /// Create the box that will store the product isotope list
  void createNuclideBox();
  /// Create the box that will store the energies for the input beam spectrum
  void createSpectrumBox();
  /// Create the box that will store the file name specifying the location of all data tables
  void createDataTablesBox();
  /// Create the box that will store the exposure and decay times
  void createTimeBox();
  /// Create the box that will store the output file information
  void createOutputBox();

  /// A function to position input buttons on the GUI
  void layOutRowButtons(QGridLayout* layout, QButtonGroup* buttons, int row = 0);

  /// Create the target isotope table given the number of rows and columns needed
  void createTargetTable(int nRows, int nColumns);

  /// A function to create a comboBox containing the given text
  QComboBox* createComboBox(const QString& text);
  /// Get the name of the output file
  QString getOutFileName(std::string prefixName, int type);

  ActOutputSelection* _outputSelection;

  QMenuBar* _menuBar;
  QGroupBox* _targetBox;
  QGroupBox* _gridGroupBox;

  QTableWidget* _targetTable;
  QGridLayout* _targetLayout;
  QGridLayout* _leftTLayout, *_rightTLayout;
  QLineEdit* _targetZEdit;

  QRadioButton* _allButton, *_ZButton;
  QLabel* _singleZLabel, *_singleALabel;
  QLineEdit* _singleZEdit, *_singleAEdit;
  QSpinBox* _isotopeSpinBox;

  QGroupBox* _calcBox;
  QGroupBox* _nuclideBox;
  QComboBox* _nuclideCombo;
  
  QGroupBox* _spectrumBox;
  QLineEdit* _minEEdit, *_maxEEdit, *_dEEdit;

  QGroupBox* _dataTablesBox;
  QRadioButton* _dataButton;
  QComboBox* _dataTablesCombo;
  QLineEdit* _minSigmaEdit;
  QPushButton* _nuclideButton;

  QGroupBox* _timeBox;
  QLineEdit* _expTimeEdit, *_decTimeEdit;

  QGroupBox* _outputBox;
  QComboBox* _xSecOutBox, *_decayOutBox;
  QButtonGroup* _xSecFormats, *_xSecDetails;
  QButtonGroup* _decayFormats, *_decayDetails;

  bool _doAll, _doSingle, _useDataTables;

};

#endif

#endif
