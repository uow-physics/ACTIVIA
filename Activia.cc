#include "Activia/ActXTermRun.hh"

#ifdef ACT_USE_QT
#include "Activia/ActGuiRun.hh"
#include "QtGui/qapplication.h"
#endif

#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;

int main(int argc, char **argv) {

  int runMethod = 0;
  if (argc > 1) {runMethod = atoi(argv[1]);}

  bool useGui(false);

  if (runMethod == 1) {

    // Use the GUI

#ifdef ACT_USE_QT
    // We must first call the QApplication constructor to initialise Qt
    QApplication application(argc, argv);

    ActGuiRun run;
    run.makeGui();
    useGui = true;
    cout<<"HERE"<<endl;
#else
    useGui = false;
#endif

  }

  if (useGui == false) {

    // Run the xterm command line version either by choice 
    // or because Qt isn't used
    ActXTermRun run;
    run.run();

  }

  return 0;

}
