#ifdef ACT_USE_QT

// Input interface
#include "Activia/ActGuiWindow.hh"

#include "QtGui/qdialog.h"
#include "QtGui/qdialogbuttonbox.h"
#include "QtCore/qdir.h"
#include "QtGui/qfiledialog.h"
#include "QtCore/qstringlist.h"

#include <iostream>
#include <fstream>
#include <string>
using std::cout;
using std::endl;
using std::cin;

ActGuiWindow::ActGuiWindow(QWidget* parent) : QWidget(parent)
{
  _outputSelection = 0;
  this->initialise();
}

ActGuiWindow::ActGuiWindow(QWidget* parent, ActOutputSelection* outputSelection) : QWidget(parent)
{
  _outputSelection = outputSelection;
  this->initialise();
}

ActGuiWindow::~ActGuiWindow()
{
}

void ActGuiWindow::initialise() {

  _menuBar = 0; _targetBox = 0; _gridGroupBox = 0;
  _targetTable = 0; _targetLayout = 0; 
  _leftTLayout = 0; _rightTLayout = 0;
  _targetZEdit = 0;
  _singleZEdit = 0; _singleAEdit = 0;
  _calcBox = 0; _nuclideBox = 0;
  _nuclideCombo = 0; _spectrumBox = 0;
  _minEEdit = 0; _maxEEdit = 0; _dEEdit = 0;
  _dataTablesBox = 0; _dataTablesCombo = 0;
  _minSigmaEdit = 0; _nuclideButton = 0;
  _timeBox = 0; _expTimeEdit = 0; _decTimeEdit = 0;
  _outputBox = 0; _xSecOutBox = 0; _decayOutBox = 0;
  _xSecFormats = 0; _xSecDetails = 0;
  _decayFormats = 0; _decayDetails = 0;

  _doAll = true; _doSingle = false; _useDataTables = false;

}

void ActGuiWindow::createBoxes() {

  this->createTargetBox();
  this->createCalcMode();
  this->createNuclideBox();
  this->createSpectrumBox();
  this->createDataTablesBox();
  this->createTimeBox();
  this->createOutputBox();
}

int ActGuiWindow::getTargetZ() {

  int targetZ(0);
  if (_targetZEdit != 0) {targetZ = _targetZEdit->text().toInt();}
  return targetZ;

}

std::string ActGuiWindow::getIsotopeFileName() {

  QString dataFileName = _nuclideCombo->itemText(0);
  std::string dataFileString = dataFileName.toStdString();
  cout<<"Input data file name is "<<dataFileString<<endl;
  return dataFileString;
}

int ActGuiWindow::getSingleZ() {

  int singleZ(0);
  if (_singleZEdit != 0) {singleZ = _singleZEdit->text().toInt();}
  return singleZ;

}

double ActGuiWindow::getSingleA() {

  double singleA(0.0);
  if (_singleAEdit != 0) {singleA = _singleAEdit->text().toDouble();}
  return singleA;

}

void ActGuiWindow::createTargetBox() {

  _targetBox = new QGroupBox(QObject::tr("Target definition"));
  _targetBox->setWhatsThis(QObject::tr("<p>Define the target element and its isotopes</p>"));

  _targetLayout = new QGridLayout();
  _targetBox->setLayout(_targetLayout);

  QGroupBox* leftBox = new QGroupBox();
  _leftTLayout = new QGridLayout();
  leftBox->setLayout(_leftTLayout);
  _targetLayout->addWidget(leftBox, 0, 0);

  QGroupBox* rightBox = new QGroupBox();
  _rightTLayout = new QGridLayout();
  rightBox->setLayout(_rightTLayout);
  _targetLayout->addWidget(rightBox, 0, 1);

  QLabel* ZLabel = new QLabel(QObject::tr("Atomic number, Z:"));
  ZLabel->setWhatsThis(QObject::tr("<p>Set target atomic number (integer)</p>"));
  _targetZEdit = new QLineEdit();
  QDoubleValidator *ZValid = new QDoubleValidator(_targetZEdit);
  _targetZEdit->setValidator(ZValid);

  _leftTLayout->addWidget(ZLabel, 0, 0);
  _leftTLayout->addWidget(_targetZEdit, 0, 1);

  int nRows(1), nColumns(2);
  this->createTargetTable(nRows, nColumns);

  // Add spin box to specify how many isotopes we want
  QLabel* isotopeLabel = new QLabel(QObject::tr("Number of isotopes"));
  isotopeLabel->setWhatsThis(QObject::tr("<p>Specify how many isotopes this target element has</p>"));
  _isotopeSpinBox = new QSpinBox();
  _isotopeSpinBox->setRange(0, 100);
  _isotopeSpinBox->setValue(nRows);

  connect(_isotopeSpinBox, SIGNAL(valueChanged(int)), 
	  this, SLOT(updateTargetTable(int)));

  _leftTLayout->addWidget(isotopeLabel, 1, 0);
  _leftTLayout->addWidget(_isotopeSpinBox, 1, 1);

  _targetLayout->setColumnStretch(0, 1);
  _targetLayout->setColumnStretch(1, 2);
  _targetLayout->setColumnMinimumWidth(1, 10);

}

void ActGuiWindow::createCalcMode() {

  _calcBox = new QGroupBox(QObject::tr("Calculation Mode"));
  _calcBox->setWhatsThis(QObject::tr("<p>Specify whether the cross-section and yields for all possible isotope"
				     " products should be calculated for the above target, or for only one "
				     "product isotope (please provide Z,A values).</p>"));
  QGridLayout* calcLayout = new QGridLayout();
  _calcBox->setLayout(calcLayout);
  
  _allButton = new QRadioButton("All isotopes");
  _allButton->setChecked(true);
  _ZButton = new QRadioButton("Single isotope");
  
  connect(_allButton, SIGNAL(toggled(bool)),
	  this, SLOT(doAll(bool)));
  connect(_ZButton, SIGNAL(toggled(bool)),
	  this, SLOT(doSingle(bool)));
  
  calcLayout->addWidget(_allButton, 0, 0);
  calcLayout->addWidget(_ZButton, 0, 1);
  
  _singleZLabel = new QLabel(QObject::tr("Isotope Z:"));
  _singleZEdit = new QLineEdit();
  QDoubleValidator* ZValid = new QDoubleValidator(_singleZEdit);
  _singleZEdit->setValidator(ZValid);
  
  _singleALabel = new QLabel(QObject::tr("Isotope A:"));
  _singleAEdit = new QLineEdit();
  QDoubleValidator* AValid = new QDoubleValidator(_singleAEdit);
  _singleAEdit->setValidator(AValid);
  
  _singleZEdit->setReadOnly(true);
  _singleAEdit->setReadOnly(true);

  _singleZLabel->setEnabled(false);
  _singleALabel->setEnabled(false);
  
  calcLayout->addWidget(_singleZLabel, 0, 2);
  calcLayout->addWidget(_singleZEdit, 0, 3);
  calcLayout->addWidget(_singleALabel, 0, 4);
  calcLayout->addWidget(_singleAEdit, 0, 5);
  
}

void ActGuiWindow::createNuclideBox() {

  _nuclideBox = new QGroupBox(QObject::tr("List of available isotopes"));
  _nuclideBox->setWhatsThis(QObject::tr("<p>Specify the input list of isotope nuclide data."
					"Each line in this file contains the Z, A and halfLife (in days) values for "
					"all allowed isotopes.</p>"
					"<p>Each line also stores the Z and A values of any side-branch (SB) nuclei:</p>"
					"<p> Z A halfLife nSB Z_1 A_1 ... Z_n A_n</p>"
					"<p>It is highly recommended that this should be the default file "
					"\"decayData.dat\".</p>"));

  QHBoxLayout* nuclideLayout = new QHBoxLayout();
  _nuclideBox->setLayout(nuclideLayout);

  QString currentPath = QDir::currentPath();
  currentPath += "/decayData.dat";
  _nuclideCombo = this->createComboBox(currentPath);
  nuclideLayout->addWidget(_nuclideCombo, 0);

  _nuclideButton = new QPushButton(tr("Browse"));
  connect(_nuclideButton, SIGNAL(clicked()), this, SLOT(getNuclideList()));
  nuclideLayout->addWidget(_nuclideButton, 1);

}

QComboBox* ActGuiWindow::createComboBox(const QString& text) {

  QComboBox* comboBox = new QComboBox();
  comboBox->setEditable(true);
  comboBox->addItem(text);
  comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  return comboBox;

}

void ActGuiWindow::createTargetTable(int nRows, int nColumns) {

  if (_targetTable != 0) {delete _targetTable;}
  _targetTable = new QTableWidget(nRows, nColumns);
  _targetTable->setWhatsThis(QObject::tr("<p>Specify the atomic mass number and abundance fraction for "
					 "each isotope. The fractions should add up to one.</p>"));

  //   // Cells in the table should only accept numbers
  //   int iRow, iCol;
  //   for (iRow = 0; iRow < nRows; iRow++) {
  //     for (iCol = 0; iCol < nColumns; iCol++) {
  //       QLineEdit* tableEdit = new QLineEdit();
  //       QDoubleValidator *tableValid = new QDoubleValidator(tableEdit);
  //       tableEdit->setValidator(tableValid);
  //       _targetTable->setCellWidget(iRow, iCol, tableEdit);
  //     }
  //   }

  QStringList labels;
  labels << QObject::tr("Atomic mass, A")
	 << QObject::tr("Abundance fraction, f (0 to 1)");
  _targetTable->setHorizontalHeaderLabels(labels);
  _targetTable->resizeColumnsToContents();
  _targetTable->adjustSize();

  // Add the table to the layout
  _rightTLayout->addWidget(_targetTable, 0, 0);

}

void ActGuiWindow::updateTargetTable(int nRows) {

  int nColumns(2);
  this->createTargetTable(nRows, nColumns);

}

void ActGuiWindow::getNuclideList() {
  
  QString dataFile = QFileDialog::getOpenFileName(this,
						  QObject::tr("Find Data File"), 
						  QDir::currentPath(),
						  QObject::tr("Data files (*.dat *.txt)"));
  if (!dataFile.isEmpty() && _nuclideCombo != 0) {
    _nuclideCombo->setItemText(0, dataFile);
  }
  
}

void ActGuiWindow::doAll(bool status) {

  _doAll = status;
  _doSingle = !_doAll;
  _singleZEdit->setReadOnly(true);
  _singleAEdit->setReadOnly(true);
  _singleZEdit->setEnabled(false);
  _singleAEdit->setEnabled(false);
  _singleZLabel->setEnabled(false);
  _singleALabel->setEnabled(false);

}

void ActGuiWindow::doSingle(bool status) {

  _doSingle = status;
  _doAll = !_doSingle;
  _singleZEdit->setReadOnly(false);
  _singleAEdit->setReadOnly(false);
  _singleZEdit->setEnabled(true);
  _singleAEdit->setEnabled(true);
  _singleZLabel->setEnabled(true);
  _singleALabel->setEnabled(true);

}

void ActGuiWindow::createSpectrumBox() {

  _spectrumBox = new QGroupBox(QObject::tr("Cosmic ray beam spectrum energy range"));
  _spectrumBox->setWhatsThis(QObject::tr("<p>Define the starting energy, end energy and bin width"
					 " for the cosmic ray beam spectrum, all in units of MeV.</p>"
					 "<p>This is needed to calculate the isotope yields for the products "
					 "using a parameterisation of the cosmic ray energy spectrum.</p>"));

  QGridLayout* spectrumLayout = new QGridLayout();
  _spectrumBox->setLayout(spectrumLayout);

  QLabel* minELabel = new QLabel(QObject::tr("Starting energy (MeV)"));
  _minEEdit = new QLineEdit();
  QDoubleValidator* minEValid = new QDoubleValidator(_minEEdit);
  _minEEdit->setValidator(minEValid);
  _minEEdit->setText("10.0");

  QLabel* maxELabel = new QLabel(QObject::tr("End-point energy (MeV)"));
  _maxEEdit = new QLineEdit();
  QDoubleValidator* maxEValid = new QDoubleValidator(_maxEEdit);
  _maxEEdit->setValidator(maxEValid);
  _maxEEdit->setText("1.0e4");

  QLabel* dELabel = new QLabel(QObject::tr("Bin width(MeV)"));
  _dEEdit = new QLineEdit();
  QDoubleValidator* dEValid = new QDoubleValidator(_dEEdit);
  _dEEdit->setValidator(dEValid);
  _dEEdit->setText("100.0");

  spectrumLayout->addWidget(minELabel, 0, 0);
  spectrumLayout->addWidget(_minEEdit, 1, 0);

  spectrumLayout->addWidget(maxELabel, 0, 1);
  spectrumLayout->addWidget(_maxEEdit, 1, 1);

  spectrumLayout->addWidget(dELabel, 0, 2);
  spectrumLayout->addWidget(_dEEdit, 1, 2);

}

double ActGuiWindow::getEStart() {

  double EStart(0.0);
  if (_minEEdit != 0) {EStart = _minEEdit->text().toDouble();}
  return EStart;

}

double ActGuiWindow::getEEnd() {

  double EEnd(0.0);
  if (_maxEEdit != 0) {EEnd = _maxEEdit->text().toDouble();}
  return EEnd;
}

double ActGuiWindow::getdE() {

  double dE(0.0);
  if (_dEEdit != 0) {dE = _dEEdit->text().toDouble();}
  return dE;
}

void ActGuiWindow::createDataTablesBox() {

  _dataTablesBox = new QGroupBox(QObject::tr("Tables of cross-section data"));
  _dataTablesBox->setWhatsThis(QObject::tr("<p>Specify whether any cross-section data files are to be used.</p>"
					   "<p>Each line in the input file should contain the individual file names "
					   "storing the cross-section data tables.</p>"
					   "<p>Each file should only store the cross-section data for one target isotope "
					   "(but any number of product isotopes).</p>"
					   "<p>The tables in each data file should have the following format:</p>"
					   "# Any starting comment lines"
					   "<br>Target Zt At</br>"
					   "<br># New table</br>"
					   "<br>Product1 Zp1 Ap1</br>"
					   "<br>E (MeV)  sigma (mb) values in two column format</br>"
					   "<br># New table</br>"
					   "<br>Product2 Zp2 Ap2</br>"
					   "<br>E (MeV)  sigma (mb) values in two column format</br>"
					   "<p>Also specify the minimum cross-section (mb) "
 					   "for the data tables (if the cross-section is less than this minimum value, "
					   "then semi-empirical formulae are used instead). "
					   "The recommended value is 0.001 mb.</p>"));

  QGridLayout* dataLayout = new QGridLayout();
  _dataTablesBox->setLayout(dataLayout);

  _dataButton = new QRadioButton("Use data tables");
  connect(_dataButton, SIGNAL(toggled(bool)),
	  this, SLOT(useDataTables(bool)));
  dataLayout->addWidget(_dataButton, 0, 0);
  
  QLabel* minSigmaLabel = new QLabel(QObject::tr("Minimum data cross-section (mb)"));
  _minSigmaEdit = new QLineEdit();
  _minSigmaEdit->setText("0.001");
  QDoubleValidator* minSigmaValid = new QDoubleValidator(_minSigmaEdit);
  _minSigmaEdit->setValidator(minSigmaValid);
  
  dataLayout->addWidget(minSigmaLabel, 1, 0);
  dataLayout->addWidget(_minSigmaEdit, 1, 1);

  QString currentPath = QDir::currentPath();
  currentPath += "/listOfDataFiles.txt";
  _dataTablesCombo = this->createComboBox(currentPath);
  dataLayout->addWidget(_dataTablesCombo, 2, 0);

  _nuclideButton = new QPushButton(tr("Browse"));
  connect(_nuclideButton, SIGNAL(clicked()), this, SLOT(getDataTables()));
  dataLayout->addWidget(_nuclideButton, 2, 1);

  // Default is no data tables
  _dataButton->setChecked(false);
  _minSigmaEdit->setEnabled(false);
  _minSigmaEdit->setReadOnly(true);
  _dataTablesCombo->setEnabled(false);
  _nuclideButton->setEnabled(false);

}

double ActGuiWindow::getMinSigma() {

  double minSigma(0.0);
  if (_minSigmaEdit != 0) {minSigma = _minSigmaEdit->text().toDouble();}
  return minSigma;

}

std::string ActGuiWindow::getDataTablesFile() {

  QString tableFileName = _dataTablesCombo->itemText(0);
  std::string tableFileString = tableFileName.toStdString();
  return tableFileString;

}

void ActGuiWindow::useDataTables(bool status) {

  _useDataTables = status;

  if (status == false) {
    _minSigmaEdit->setReadOnly(true);
    _minSigmaEdit->setEnabled(false);
    _dataTablesCombo->setEnabled(false);
    _nuclideButton->setEnabled(false);
  } else {
    _minSigmaEdit->setReadOnly(false);
    _minSigmaEdit->setEnabled(true);
    _dataTablesCombo->setEnabled(true);
    _nuclideButton->setEnabled(true);
  }

}

void ActGuiWindow::getDataTables() {
  
  QString dataFile = QFileDialog::getOpenFileName(this,
						  QObject::tr("Find file containing list of data tables"), 
						  QDir::currentPath(),
						  QObject::tr("Input file (*.txt *.dat)"));
  if (!dataFile.isEmpty() && _dataTablesCombo != 0) {
    _dataTablesCombo->setItemText(0, dataFile);
  }
  
}

void ActGuiWindow::createTimeBox() {

  _timeBox = new QGroupBox(QObject::tr("Beam exposure and decay times"));
  _timeBox->setWhatsThis(QObject::tr("<p>Specify the (cosmic ray) beam exposure and decay times (in days)"
				     " for calculating the yields of the isotope products"
				     " created by cosmic rays hitting the target.</p>"
				     "<p>A simple decay yield calculation is used "
				     "(no chained/multiple decays).</p>"));

  QHBoxLayout* timeLayout = new QHBoxLayout();
  _timeBox->setLayout(timeLayout);
  
  QLabel* expTimeLabel = new QLabel(QObject::tr("Beam exposure time (days):"));
  _expTimeEdit = new QLineEdit();
  _expTimeEdit->setText("0.0");
  QDoubleValidator* expTimeValid = new QDoubleValidator(_expTimeEdit);
  _expTimeEdit->setValidator(expTimeValid);
  
  timeLayout->addWidget(expTimeLabel, 0);
  timeLayout->addWidget(_expTimeEdit, 1);

  QLabel* decTimeLabel = new QLabel(QObject::tr("Decay (cooling) time (days):"));
  _decTimeEdit = new QLineEdit();
  _decTimeEdit->setText("0.0");
  QDoubleValidator* decTimeValid = new QDoubleValidator(_decTimeEdit);
  _decTimeEdit->setValidator(decTimeValid);
  
  timeLayout->addWidget(decTimeLabel, 2);
  timeLayout->addWidget(_decTimeEdit, 3);

}

double ActGuiWindow::getExposureTime() {

  double texp(0.0);
  if (_expTimeEdit != 0) {texp = _expTimeEdit->text().toDouble();}
  return texp;

}

double ActGuiWindow::getDecayTime() {

  double tdec(0.0);
  if (_decTimeEdit != 0) {tdec = _decTimeEdit->text().toDouble();}
  return tdec;

}

void ActGuiWindow::createOutputBox() {

  // Select the output filenames, formats and level of detail. Update the ActOutputSelection
  // pointer directly.
  if (_outputSelection == 0) {return;}

  // Group box for the output
  _outputBox = new QGroupBox(QObject::tr("Output filenames, format and level of detail"));
  _outputBox->setWhatsThis(QObject::tr("<p>Specify the output filenames, format and level of detail.</p>"));

  // Grid layout
  QGridLayout* outputLayout = new QGridLayout();
  _outputBox->setLayout(outputLayout);

  // Cross-section output sub-box
  QGroupBox* xSecGroupBox = new QGroupBox(QObject::tr("Cross-sections output file:"));
  xSecGroupBox->setWhatsThis(QObject::tr("<p>Specify the output for the cross-section calculations.</p>"));
  QGridLayout* xSecLayout = new QGridLayout();
  xSecGroupBox->setLayout(xSecLayout);
  outputLayout->addWidget(xSecGroupBox);

  // Decay yield output sub-box
  QGroupBox* decayGroupBox = new QGroupBox(QObject::tr("Decay yields output file:"));
  decayGroupBox->setWhatsThis(QObject::tr("<p>Specify the output for the decay yield calculations.</p>"));
  QGridLayout* decayLayout = new QGridLayout();
  decayGroupBox->setLayout(decayLayout);
  outputLayout->addWidget(decayGroupBox);

  // Find out which formats are available
  _xSecFormats = new QButtonGroup(); _xSecDetails = new QButtonGroup();
  _decayFormats = new QButtonGroup(); _decayDetails = new QButtonGroup();

  ActOutputSelection::ActOutputMap outputFormats = _outputSelection->getAllowedTypes();
  ActOutputSelection::ActOutputMap::iterator iter;
  int defaultType = _outputSelection->getDefaultType();

  for (iter = outputFormats.begin(); iter != outputFormats.end(); ++iter) {

    int typeFlag = iter->first;
    std::string typeName = iter->second;
    QString typeString(typeName.c_str());

    QRadioButton* xSecFormatButton = new QRadioButton(typeString);
    if (typeFlag == defaultType) {xSecFormatButton->setChecked(true);}
    _xSecFormats->addButton(xSecFormatButton, typeFlag); // id = output flag integer

    QRadioButton* decayFormatButton = new QRadioButton(typeString);
    if (typeFlag == defaultType) {decayFormatButton->setChecked(true);}
    _decayFormats->addButton(decayFormatButton, typeFlag);

  }
  
  connect(_xSecFormats, SIGNAL(buttonClicked(QAbstractButton*)),
	  this, SLOT(getXSecFormat(QAbstractButton*)));

  connect(_decayFormats, SIGNAL(buttonClicked(QAbstractButton*)),
	  this, SLOT(getDecayFormat(QAbstractButton*)));

  // Find out which output details are available
  ActOutputSelection::ActOutputMap outputDetails = _outputSelection->getAllowedDetails();
  int defaultDetail = _outputSelection->getDefaultDetail();

  for (iter = outputDetails.begin(); iter != outputDetails.end(); ++iter) {

    int detailFlag = iter->first;
    std::string detailName = iter->second;
    QString detailString(detailName.c_str());

    QRadioButton* xSecDetailButton = new QRadioButton(detailString);
    if (detailFlag == defaultDetail) {xSecDetailButton->setChecked(true);}
    _xSecDetails->addButton(xSecDetailButton, detailFlag); // id = output flag integer

    QRadioButton* decayDetailButton = new QRadioButton(detailString);
    if (detailFlag == defaultDetail) {decayDetailButton->setChecked(true);}
    _decayDetails->addButton(decayDetailButton, detailFlag);

  }

  connect(_xSecDetails, SIGNAL(buttonClicked(QAbstractButton*)),
	  this, SLOT(getXSecDetail(QAbstractButton*)));

  connect(_decayDetails, SIGNAL(buttonClicked(QAbstractButton*)),
	  this, SLOT(getDecayDetail(QAbstractButton*)));

  // Layout the format/detail buttons along rows
  this->layOutRowButtons(xSecLayout, _xSecFormats, 0);
  this->layOutRowButtons(xSecLayout, _xSecDetails, 1);

  this->layOutRowButtons(decayLayout, _decayFormats, 0);
  this->layOutRowButtons(decayLayout, _decayDetails, 1);

  // Put in default output file name widgets

  // Cross-section output file name
  QString xSecOutFileName = this->getOutFileName("xSecOutput", defaultType);
  _xSecOutBox = this->createComboBox(xSecOutFileName);
  _outputSelection->setXSecFileName(xSecOutFileName.toStdString());

  // Store name in the output selection class. 
  xSecLayout->addWidget(_xSecOutBox, 2, 0);

  // Cross-section file browser
  QPushButton* xSecOutBrowse = new QPushButton(QObject::tr("Browse"));
  connect(xSecOutBrowse, SIGNAL(clicked()), this, SLOT(getXSecOutFile()));
  xSecLayout->addWidget(xSecOutBrowse, 2, 1);

  // Make sure any edit to the output file name is picked up and propagated
  // to the outputSelection. Do this using the editTextChanged signal.
  connect(_xSecOutBox, SIGNAL(editTextChanged(const QString&)), this,
	  SLOT(setXSecOutFileName(const QString&)));

  // Decay yield output file name
  QString decayOutFileName = this->getOutFileName("decayOutput", defaultType);
  _decayOutBox = this->createComboBox(decayOutFileName);
  _outputSelection->setDecayFileName(decayOutFileName.toStdString());

  // Store name in the output selection class
  decayLayout->addWidget(_decayOutBox, 2, 0);

  // Decay yield file browser
  QPushButton* decayOutBrowse = new QPushButton(QObject::tr("Browse"));
  connect(decayOutBrowse, SIGNAL(clicked()), this, SLOT(getDecayOutFile()));
  decayLayout->addWidget(decayOutBrowse, 2, 1);

  // Make sure any edit to the output file name is picked up and propagated
  // to the outputSelection. Do this using the editTextChanged signal.
  connect(_decayOutBox, SIGNAL(editTextChanged(const QString&)), this,
	  SLOT(setDecayOutFileName(const QString&)));

}

void ActGuiWindow::getXSecOutFile() {

  // Specify the output file name for the cross-section results.
  // Since we want to specify a filename that may not already exist,
  // we cannot use getOpenFileName directly.

  QFileDialog dialog(this, QObject::tr("Specify cross-section output file"),
		     QDir::currentPath(),
		     QObject::tr("Output files (*.out *.root)"));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setConfirmOverwrite(true);

  QString outFile("");

  if (dialog.exec() == QDialog::Accepted) {

    QStringList selectedFiles = dialog.selectedFiles();

    if (selectedFiles.count() > 0) {
      outFile = selectedFiles.first();
    }
  }

  if (!outFile.isEmpty() && _xSecOutBox != 0) {
    _xSecOutBox->setItemText(0, outFile);
    // Reset the output file name
    _outputSelection->setXSecFileName(outFile.toStdString());
  }

}

void ActGuiWindow::getDecayOutFile() {

  // Specify the output file name for the cross-section results.
  // Since we want to specify a filename that may not already exist,
  // we cannot use getOpenFileName directly.

  QFileDialog dialog(this, QObject::tr("Specify decay yield output file"),
		     QDir::currentPath(),
		     QObject::tr("Output files (*.out *.root)"));
  dialog.setFileMode(QFileDialog::AnyFile);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  dialog.setConfirmOverwrite(true);

  QString outFile("");

  if (dialog.exec() == QDialog::Accepted) {

    QStringList selectedFiles = dialog.selectedFiles();

    if (selectedFiles.count() > 0) {
      outFile = selectedFiles.first();
    }
  }

  if (!outFile.isEmpty() && _decayOutBox != 0) {
    _decayOutBox->setItemText(0, outFile);
    // Reset the output file name
    _outputSelection->setDecayFileName(outFile.toStdString());
  }

}

QString ActGuiWindow::getOutFileName(std::string prefixName, int type) {

  QString currentPath = QDir::currentPath();
  QString fileName(currentPath); fileName += "/";
  fileName += prefixName.c_str();
  if (_outputSelection != 0) {
    fileName += _outputSelection->setExtension(type).c_str();
  } else {
    fileName += ".out";
  }
  return fileName;
}

void ActGuiWindow::layOutRowButtons(QGridLayout* layout, QButtonGroup* buttons, int row) {

  QList<QAbstractButton*> list = buttons->buttons();
  QList<QAbstractButton*>::iterator iter;
  int index(-1);
  for (iter = list.begin(); iter != list.end(); ++iter) {

    index++;
    QAbstractButton* theButton = *iter;
    layout->addWidget(theButton, row, index);

  }

}

void ActGuiWindow::getXSecFormat(QAbstractButton* button) {

  if (button != 0 && _xSecFormats != 0) {
    int formatType = _xSecFormats->id(button);
    if (_outputSelection != 0) {
      _outputSelection->setXSecType(formatType);
    }
  }

}

void ActGuiWindow::getDecayFormat(QAbstractButton* button) {

  if (button != 0 && _decayFormats != 0) {
    int formatType = _decayFormats->id(button);
    if (_outputSelection != 0) {
      _outputSelection->setDecayType(formatType);
    }
  }

}

void ActGuiWindow::getXSecDetail(QAbstractButton* button) {

  if (button != 0 && _xSecDetails != 0) {
    int detail = _xSecDetails->id(button);
    if (_outputSelection != 0) {
      _outputSelection->setXSecDetail(detail);
    }
  }

}

void ActGuiWindow::getDecayDetail(QAbstractButton* button) {

  if (button != 0 && _decayDetails != 0) {
    int detail = _decayDetails->id(button);
    if (_outputSelection != 0) {
      _outputSelection->setDecayDetail(detail);
    }
  }

}

void ActGuiWindow::setXSecOutFileName(const QString &text) {

  _outputSelection->setXSecFileName(text.toStdString());

}

void ActGuiWindow::setDecayOutFileName(const QString &text) {

  _outputSelection->setDecayFileName(text.toStdString());

}

void ActGuiWindow::saveInput(const QString& fileName) {

  // Save the various numbers and strings within the GUI into an output file
  // that can be reloaded (avoiding the need to re-type the quantities).
  std::string theFile = fileName.toStdString();
  std::ofstream saveData(theFile.c_str());
  
  // Write out target isotope information
  int targetZ(0);
  if (_targetZEdit != 0) {targetZ = _targetZEdit->text().toInt();}
  saveData << targetZ << endl;

  int nIsotopes(0);
  if (_targetTable != 0) {nIsotopes = _targetTable->rowCount();}
  saveData << nIsotopes << endl;

  int iIso;
  for (iIso = 0; iIso < nIsotopes; iIso++) {

    QTableWidgetItem* AEntry = _targetTable->item(iIso, 0);
    QTableWidgetItem* fEntry = _targetTable->item(iIso, 1);
    if (AEntry != 0 && fEntry != 0) {

      double A = AEntry->text().toDouble();
      double fraction = fEntry->text().toDouble();
      saveData << A << " " << fraction << endl;
    } else {
      saveData << "0.0 0.0" << endl;
    }
  }

  // Write out calculation mode
  if (_doSingle == true) {
    saveData << "z" << endl;
  } else {
    saveData << "a" << endl;
  }

  // Write out isotope data file (and single isotope numbers if required)
  std::string isotopeFile("");
  if (_nuclideCombo != 0) {isotopeFile = _nuclideCombo->itemText(0).toStdString();}
  saveData << isotopeFile.c_str() << endl;

  if (_doSingle == true) {   
    int singleZ = this->getSingleZ();
    double singleA = this->getSingleA();
    saveData << singleZ << " " << singleA << endl;
  }

  // Write out beam energies
  double EStart = this->getEStart();
  double EEnd = this->getEEnd();
  double dE = this->getdE();
  saveData << EStart << " " << EEnd << " " << dE << endl;

  // Write out data table file
  if (_useDataTables == true) {

    std::string dataTablesFile = this->getDataTablesFile();
    double minDataXSec = this->getMinSigma();

    saveData << dataTablesFile.c_str() << " " << minDataXSec << endl;

  } else {

    saveData << "0" << endl;

  }

  // Write out decay times
  double texp = this->getExposureTime();
  double tdec = this->getDecayTime();
  saveData << texp << " " << tdec << endl;

  // Finally, write out the output files, formats and level of detail options
  std::string xSecOutFile(""), decayOutFile("");
  int xSecFormat(0), xSecDetail(0);
  int decayFormat(0), decayDetail(0);

  if (_outputSelection != 0) {

    xSecOutFile = _outputSelection->getXSecFileName();
    decayOutFile = _outputSelection->getDecayFileName();
    xSecFormat = _outputSelection->getXSecType();
    xSecDetail = _outputSelection->getXSecDetail();
    decayFormat = _outputSelection->getDecayType();
    decayDetail = _outputSelection->getDecayDetail();

  }

  saveData << xSecOutFile.c_str()  << " " << xSecFormat  << " " << xSecDetail  << endl;
  saveData << decayOutFile.c_str() << " " << decayFormat << " " << decayDetail << endl;

}

void ActGuiWindow::loadInput(const QString& fileName) {

  // Load the various numbers and strings within the GUI using the input file.
  // The format of the input file follows that defined in the saveInput() function.
  
  std::string theFile = fileName.toStdString();
  std::ifstream loadData(theFile.c_str());
  
  // Get the target definition
  int targetZ(0);
  loadData >> targetZ;
  _targetZEdit->setText(QString::number(targetZ));

  int nIsotopes(0);
  loadData >> nIsotopes;
  this->updateTargetTable(nIsotopes);
  _isotopeSpinBox->setValue(nIsotopes);

  int iIso;
  double At(0.0), fraction(0.0);
  for (iIso = 0; iIso < nIsotopes; iIso++) {
    
    loadData >> At >> fraction;

    QTableWidgetItem* AEntry = _targetTable->item(iIso, 0);
    QTableWidgetItem* fEntry = _targetTable->item(iIso, 1);

    if (AEntry != 0) {
      AEntry->setText(QString::number(At));
    } else {
      QTableWidgetItem* newAEntry = new QTableWidgetItem(QString::number(At));
      _targetTable->setItem(iIso, 0, newAEntry);
    }
     
    if (fEntry != 0) {
      fEntry->setText(QString::number(fraction));
    } else {
      QTableWidgetItem* newfEntry = new QTableWidgetItem(QString::number(fraction));
      _targetTable->setItem(iIso, 1, newfEntry);
    }

  }

  // Specify the calculation mode
  std::string calcType;
  loadData >> calcType;
  if (calcType == "z") {

    this->doSingle(true);
    _ZButton->setChecked(true);

  } else {

    this->doAll(true);
    _allButton->setChecked(true);

  }
  
  // Specify isotope data file
  std::string isotopeDataFile("");
  loadData >> isotopeDataFile;
  if (_nuclideCombo != 0) {
    QString isotopeString(isotopeDataFile.c_str());
    _nuclideCombo->setItemText(0, isotopeString);
  }  

  // Specify single product Z,A (if needed)
  if (_doSingle == true) {

    int prodZ(0);
    double prodA(0.0);
    loadData >> prodZ >> prodA;
    _singleZEdit->setText(QString::number(prodA));
    _singleAEdit->setText(QString::number(prodZ));
  }

  // Specify beam energies
  double EStart(0.0), EEnd(0.0), dE(0.0);
  loadData >> EStart >> EEnd >> dE;
  _minEEdit->setText(QString::number(EStart));
  _maxEEdit->setText(QString::number(EEnd));
  _dEEdit->setText(QString::number(dE));

  // Specify input data tables of cross-section data (if any)
  std::string dataTablesFile("");
  loadData >> dataTablesFile;

  if (dataTablesFile == "0") {

    _dataButton->setChecked(false);
    _minSigmaEdit->setEnabled(false);
    _minSigmaEdit->setReadOnly(true);
    _dataTablesCombo->setEnabled(false);
    _nuclideButton->setEnabled(false); 

  } else {

    double minSigma(0.0);
    loadData >> minSigma;

    _dataButton->setChecked(true);
    _minSigmaEdit->setEnabled(true);
    _minSigmaEdit->setReadOnly(false);
    _minSigmaEdit->setText(QString::number(minSigma));
    _dataTablesCombo->setEnabled(true);

    QString tableString(dataTablesFile.c_str());
    _dataTablesCombo->setItemText(0, tableString);
    _nuclideButton->setEnabled(true);

  }

  // Beam exposure and decay times
  double texp(0.0), tdec(0.0);
  loadData >> texp >> tdec;
  _expTimeEdit->setText(QString::number(texp));
  _decTimeEdit->setText(QString::number(tdec));

  // Finally, the output files, format and level of detail
  std::string xSecOutFile(""), decayOutFile("");
  int xSecFormat(0), xSecDetail(0);
  int decayFormat(0), decayDetail(0);

  loadData >> xSecOutFile  >> xSecFormat  >> xSecDetail;
  loadData >> decayOutFile >> decayFormat >> decayDetail;

  QString xSecOutString(xSecOutFile.c_str());
  _xSecOutBox->setItemText(0, xSecOutString);
  _outputSelection->setXSecFileName(xSecOutFile);

  QString decayOutString(decayOutFile.c_str());
  _decayOutBox->setItemText(0, decayOutString);
  _outputSelection->setDecayFileName(decayOutFile);

  QAbstractButton* xSecFormatButton = _xSecFormats->button(xSecFormat);
  if (xSecFormatButton != 0) {
    xSecFormatButton->setChecked(true);
    _outputSelection->setXSecType(xSecFormat);
  }

  QAbstractButton* decayFormatButton = _decayFormats->button(decayFormat);
  if (decayFormatButton != 0) {
    decayFormatButton->setChecked(true);
    _outputSelection->setDecayType(decayFormat);
  }

  QAbstractButton* xSecDetailButton = _xSecDetails->button(xSecDetail);
  if (xSecDetailButton != 0) {
    xSecDetailButton->setChecked(true);
    _outputSelection->setXSecDetail(xSecDetail);
  }

  QAbstractButton* decayDetailButton = _decayDetails->button(decayDetail);
  if (decayDetailButton != 0) {
    decayDetailButton->setChecked(true);
    _outputSelection->setDecayDetail(decayDetail);
  }

}

void ActGuiWindow::clearInput() {

  // Clear the GUI input

  // Clear target isotopes
  _targetZEdit->setText("");
  _isotopeSpinBox->setValue(1);
  this->updateTargetTable(1);

  // Reset calculation mode
  this->doAll(true);
  _singleZEdit->setText("");
  _singleAEdit->setText("");
  _allButton->setChecked(true);

  // Reset isotope list
  QString decayFile = QDir::currentPath();
  decayFile += "/decayData.dat";
  _nuclideCombo->setItemText(0, decayFile);

  // Clear energy range
  _minEEdit->setText("");
  _maxEEdit->setText("");
  _dEEdit->setText("");

  // Don't use data tables
  _dataButton->setChecked(false);
  _minSigmaEdit->setEnabled(false);
  _minSigmaEdit->setReadOnly(true);
  _dataTablesCombo->setEnabled(false);
  _nuclideButton->setEnabled(false);

  // Clear decay times
  _expTimeEdit->setText("");
  _decTimeEdit->setText("");

  // Clear output files
  int defaultType = _outputSelection->getDefaultType();
  int defaultDetail = _outputSelection->getDefaultDetail();

  _xSecOutBox->setItemText(0, "");
  _outputSelection->setXSecFileName("");

  _decayOutBox->setItemText(0, "");
  _outputSelection->setDecayFileName("");

  QAbstractButton* xSecFormatButton = _xSecFormats->button(defaultType);
  if (xSecFormatButton != 0) {
    xSecFormatButton->setChecked(true);
    _outputSelection->setXSecType(defaultType);
  }

  QAbstractButton* decayFormatButton = _decayFormats->button(defaultType);
  if (decayFormatButton != 0) {
    decayFormatButton->setChecked(true);
    _outputSelection->setDecayType(defaultType);
  }

  QAbstractButton* xSecDetailButton = _xSecDetails->button(defaultDetail);
  if (xSecDetailButton != 0) {
    xSecDetailButton->setChecked(true);
    _outputSelection->setXSecDetail(defaultDetail);
  }

  QAbstractButton* decayDetailButton = _decayDetails->button(defaultDetail);
  if (decayDetailButton != 0) {
    decayDetailButton->setChecked(true);
    _outputSelection->setDecayDetail(defaultDetail);
  }
  
}

#endif
