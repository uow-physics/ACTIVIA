#ifdef ACT_USE_QT

// Class to run the Activia code using a GUI interface
#include "Activia/ActGuiRun.hh"
#include "Activia/ActGuiInput.hh"
#include "Activia/ActQtDebugStream.hh"

#include "Activia/ActTarget.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActGuiCalcStatus.hh"

#include "QtGui/qaction.h"
#include "QtGui/qdialogbuttonbox.h"
#include "QtGui/qlayout.h"
#include "QtGui/qmenu.h"
#include "QtGui/qgroupbox.h"
#include "QtCore/qcoreapplication.h"
#include "QtGui/qmessagebox.h"
#include "QtGui/qfiledialog.h"
#include "QtGui/qscrollarea.h"

#include <iostream>
using std::cout;
using std::endl;

ActGuiRun::ActGuiRun() : QDialog(), ActAbsRun() {
  // Constructor
  _calcStatus = new ActGuiCalcStatus(this);
  _progressBar = new QProgressBar();
  _guiWindow = 0;

}

ActGuiRun::~ActGuiRun() {
  // Destructor
  if (_guiWindow != 0) {delete _guiWindow;}
  if (_progressBar != 0) {delete _progressBar;}
}

void ActGuiRun::makeGui() {

  _guiWindow = new ActGuiWindow(this, _outputSelection);
  _guiWindow->createBoxes();

  QVBoxLayout* mainLayout = new QVBoxLayout();
  mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

  QMenuBar* menuBar = this->createMenu();
  mainLayout->setMenuBar(menuBar);

  QGroupBox* targetBox = _guiWindow->getTargetBox();
  QGroupBox* calcBox = _guiWindow->getCalcBox();
  QGroupBox* nuclideBox = _guiWindow->getNuclideBox();
  QGroupBox* spectrumBox = _guiWindow->getSpectrumBox();
  QGroupBox* dataTablesBox = _guiWindow->getDataTablesBox();
  QGroupBox* timeBox = _guiWindow->getTimeBox();
  QGroupBox* outputBox = _guiWindow->getOutputBox();

  mainLayout->addWidget(targetBox, 2, 0);
  mainLayout->addWidget(calcBox);
  mainLayout->addWidget(nuclideBox);
  mainLayout->addWidget(spectrumBox);
  mainLayout->addWidget(dataTablesBox);
  mainLayout->addWidget(timeBox);
  mainLayout->addWidget(outputBox);

  QPushButton* runButton = new QPushButton(QObject::tr("&Run"));
  QPushButton* stopButton = new QPushButton(QObject::tr("&Stop"));
  QPushButton* closeButton = new QPushButton(QObject::tr("&Close"));

  QDialogButtonBox* buttonBox = new QDialogButtonBox();
  buttonBox->addButton(runButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(stopButton, QDialogButtonBox::ActionRole);
  buttonBox->addButton(closeButton, QDialogButtonBox::RejectRole);

  connect(runButton, SIGNAL(clicked()), this, SLOT(runCode()));
  connect(stopButton, SIGNAL(clicked()), this, SLOT(stopCode()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
 
  mainLayout->addWidget(buttonBox, 1, 0);
  this->setLayout(mainLayout);

  this->setWindowTitle(QObject::tr("ACTIVIA"));

  this->setWhatsThis(QObject::tr("<p>GUI for setting up cosmogenic isotope "
				 "production calculations that uses Silberberg-Tsao "
				 "semi-empirical formulae and cross-section data tables.</p>"));

  // Put the whole GUI window in a scroll area to make sure the user
  // can change the dimensions of the GUI to fit the screen
  QScrollArea* area = new QScrollArea();
  area->setWidget(this);
  area->setWidgetResizable(true);
  area->show();

  // Execute the GUI code
  this->exec();

}

QMenuBar* ActGuiRun::createMenu() {

  QMenuBar* menuBar = new QMenuBar();

  QMenu* fileMenu = new QMenu(QObject::tr("&File"), this);
  QAction* clearAction = fileMenu->addAction(QObject::tr("&Clear input"));
  QAction* loadAction = fileMenu->addAction(QObject::tr("&Load input"));
  QAction* saveAction = fileMenu->addAction(QObject::tr("&Save input"));
  QAction* exitAction = fileMenu->addAction(QObject::tr("E&xit GUI"));
  menuBar->addMenu(fileMenu);

  QMenu* helpMenu = new QMenu(QObject::tr("&Help"), this);
  QAction* aboutAction = helpMenu->addAction(QObject::tr("&About"));
  QAction* licenseAction = helpMenu->addAction(QObject::tr("&License"));
  QAction* aboutQtAction = helpMenu->addAction(QObject::tr("About &Qt"));
  menuBar->addMenu(helpMenu);

  connect(clearAction, SIGNAL(triggered()), this, SLOT(clearInput()));
  connect(loadAction, SIGNAL(triggered()), this, SLOT(loadInput()));
  connect(saveAction, SIGNAL(triggered()), this, SLOT(saveInput()));
  connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));

  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
  connect(licenseAction, SIGNAL(triggered()), this, SLOT(license()));
  connect(aboutQtAction, SIGNAL(triggered()), QCoreApplication::instance(), SLOT(aboutQt()));

  return menuBar;

}

void ActGuiRun::runCode() {

  // Slot for catching the "run the code" signal.
  // This just calls the ActAbsRun::run() command, which sets up 
  // the input and performs the calculations.

  // Set the calculation status window
  _calcStatus->setUp();

  _calcStatus->setRunCode(true);

  // Run the ACTIVIA code
  this->run();

}

void ActGuiRun::stopCode() {

  // Stop running the calculation code, but keep the GUI open. 
  // The ActProdXSecData and ActDecayAlgorithm classes
  // know when to stop the code when this is set to false.
  // When the code is restarted, all internal data should be
  // reinitialised from the provided input.
  _calcStatus->setRunCode(false);

}

void ActGuiRun::defineInput() {

  // Define the calculation inputs by extracting data from the GUI
  _input = new ActGuiInput(_outputSelection, _guiWindow);

}

void ActGuiRun::about() {

  QMessageBox::about(this, QObject::tr("About Activia"),
		     QObject::tr("<p> <h3> Activia - Calculation of isotope production cross-sections and yields </h3>"
 				 "<p> Version 1.3.1 - developed by John Back and Yorck Ramachers </p>"
	 			 "<p> Copyright (C) 2007-2016 University of Warwick</p>"
				 "<p> See <a href=\"https://github.com/UniversityofWarwick/ACTIVIA\">"
				 "https://github.com/UniversityofWarwick/ACTIVIA</a> for more information.</p>"));
}

void ActGuiRun::license() {

  QMessageBox::about(this, QObject::tr("License"),
		     QObject::tr("<h3> Activia: Calculation of Isotope Production Cross-Sections and Yields </h3>"
				 "<p>Copyright (C) 2007-2016, University of Warwick.</p>"
				 "<p>Permission is hereby granted, free of charge, to any person or organization "
				 "obtaining a copy of the software and accompanying documentation covered by "
				 "this license (the \"Software\") to use, reproduce, display, distribute, "
				 "execute, and transmit the Software, and to prepare derivative works of the "
				 "Software, and to permit third-parties to whom the Software is furnished to "
				 "do so, all subject to the following:</p>"				 
				 "<p>The copyright notices in the Software and this entire statement, including "
				 "the above license grant, this restriction and the following disclaimer, "
				 "must be included in all copies of the Software, in whole or in part, and "
				 "all derivative works of the Software, unless such copies or derivative "
				 "works are solely in the form of machine-executable object code generated by "
				 "a source language processor.</p>"
				 "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
				 "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
				 "FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT "
				 "SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE "
				 "FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, "
				 "ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER "
				 "DEALINGS IN THE SOFTWARE.</p>"));

}

void ActGuiRun::clearInput() {

  // Clear the GUI input
  _guiWindow->clearInput();

}


void ActGuiRun::saveInput() {

  // Save the numbers/words in the GUI window into a file that can be read
  // back in by loadInput.

  QString fileName = QFileDialog::getSaveFileName(this, 
						  QObject::tr("Save GUI input"),
						  QDir::currentPath(),
						  QObject::tr("Input file (*.in *.txt *.dat)"));
  if (!fileName.isEmpty()) {
    _guiWindow->saveInput(fileName);
  }

}

void ActGuiRun::loadInput() {

  // Re-load the GUI numbers/words using an input file
  QString fileName = QFileDialog::getOpenFileName(this, 
						  QObject::tr("Load GUI input"),
						  QDir::currentPath(),
						  QObject::tr("Input file (*.in *.txt *.dat)"));
  if (!fileName.isEmpty()) {
    _guiWindow->loadInput(fileName);
  }

}

#endif
