#ifdef ACT_USE_QT

#ifndef ACT_GUI_RUN_HH
#define ACT_GUI_RUN_HH

#include "Activia/ActAbsRun.hh"
#include "Activia/ActGuiWindow.hh"

#include "QtGui/qdialog.h"
#include "QtGui/qmenubar.h"
#include "QtGui/qprogressbar.h"

/// \brief Run all of the isotope production code using a GUI
///
/// All relevent input and output classes as well as the 
/// cross-section and isotope yield calculations will be called 
/// here using the abstract ActAbsRun interface.

class ActGuiRun : public QDialog, public ActAbsRun {

  Q_OBJECT

public:

  /// Constructor
  ActGuiRun();
  
  /// Destructor
  virtual ~ActGuiRun();
  
  /// Define how the input to the calculations are obtained.
  virtual void defineInput();  
  
  /// Make the GUI and run the calculations
  void makeGui();

public slots:

  /// Run the calculations
  void runCode();
  /// Stop the calculations
  void stopCode();
  /// Create pop-up window with brief description about the software
  void about();
  /// Create pop-up window showing the usage license for the code
  void license();

  /// Clear all GUI inputs
  void clearInput();
  /// Save the GUI inputs
  void saveInput();
  /// Load the GUI inputs
  void loadInput();

protected:

private:

  QProgressBar* _progressBar;
  ActGuiWindow* _guiWindow;

  /// Create the top menu bar for the GUI
  QMenuBar* createMenu();

};

#endif

#endif
