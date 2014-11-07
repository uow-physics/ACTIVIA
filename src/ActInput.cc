// Input interface

#include "Activia/ActInput.hh"
#include "Activia/ActProdNuclideList.hh"
#include "Activia/ActProdNuclide.hh"
#include "Activia/ActTarget.hh"
#include "Activia/ActTime.hh"
#include "Activia/ActCosmicSpectrum.hh"
#include "Activia/ActSTXSecAlgorithm.hh"
#include "Activia/ActSimpleDecayAlgorithm.hh"

#include <cstdlib>
#include <string>
#include <map>
#include <fstream>

using std::cout;
using std::endl;
using std::cin;

ActInput::ActInput() : ActAbsInput()
{
  _prodDataFile = "";
  this->setUp();
}

ActInput::ActInput(ActOutputSelection* outputSelection) : ActAbsInput(outputSelection)
{
  _prodDataFile = "";
  this->setUp();
}

ActInput::~ActInput() 
{
}

void ActInput::setUp() {

  // Calculation option
  _calcInt = ActAbsInput::AllProducts; _option = "a";
  _prodDataFile = "decayData.dat";

  // To turn off the introduction preamble, comment out this line
  this->printIntro();

  cout.flags(std::ios::right);
  cout.flags(std::ios::fixed);

  cout<<"The program will ask a series of questions to set-up the calculations."<<endl;
  cout<<"Please separate multiple values on a single line by spaces\n"<<endl;

}

void ActInput::defineTarget() {

  std::string tmpChar;

  int zt, nisott;
  cout<<"Enter target Z"<<endl;
  cin >> zt;

  cout<<"Enter number of target isotopes"<<endl;
  cin >> nisott;

  // Set the target pointer
  if (_target != 0) {delete _target;}
  _target = new ActTarget(zt);

  // Add the various isotopes to this target
  int i;
  for (i = 0; i < nisott; i++) {

    double massNo = 0.0;
    double ratio = 0.0;
    double halfLife = 0.0;
    
    cout<<"Enter mass number and fractional abundance (0 to 1) for the target isotope "<<i+1<<endl;
    cout<<"For example: 180 0.763"<<endl;
    cin >> massNo >> ratio;
    _target->addIsotope(massNo, ratio, halfLife);

  }
}

void ActInput::defineCalcMode() {

  // Decide on calculation mode
  std::string tmpChar;

  bool gotMode = false;
  while (gotMode == false) {

    cout<<endl;
    cout<<"Enter z for calculating the cross-section and yield for one isotope product only "
	<<"or a for finding the cross-sections and yields for all available active products"<<endl;
    cout<<"z or a: ";
    cin >> tmpChar;

    if (tmpChar == "z" || tmpChar == "Z") {
      _calcInt = ActAbsInput::SingleProd; gotMode = true;
      _option = "z";
    } else if (tmpChar == "a" || tmpChar == "A") {
      _calcInt = ActAbsInput::AllProducts; gotMode = true;
      _option = "a";
    }

    cout<<"gotMode = "<<(int) gotMode<<endl;

  }

}

void ActInput::defineNuclides() {

  if (_prodNuclideList != 0) {delete _prodNuclideList;}
  _prodNuclideList = new ActProdNuclideList();

  // Input options now depend on the calculation mode
  std::string tmpChar;

  cout<<endl;
  cout<<"Enter the filename defining all possible product isotopes and their side-branches"
      <<" or type 0 to accept the default file "<<_prodDataFile<<":"<<endl;
  cin >> tmpChar;

  if (tmpChar == "0") {

    cout<<"Leaving the product isotope data file to be "<<_prodDataFile<<"\n"<<endl;

  } else {

    // Check if the file exists. If not, ask the user to try another name
    bool ok(false);

    int iTry(0);
    for (iTry = 0; iTry < 10; iTry++) {

      _prodDataFile = tmpChar;
      std::ifstream tmpStream(_prodDataFile.c_str());

      if (tmpStream.is_open()) {
	ok = true;
	break;
      } else {
	cout<<"Error. The file "<<_prodDataFile<<" does not exist. Please enter another file\n"<<endl;
	cin >> tmpChar;
      }
    }

    if (ok == false) {
      cout<<"Stopping program since we can not find the isotope data file"<<endl;
      exit(-1);
    }

    cout<<"Setting the product isotope data file to be "<<_prodDataFile<<"\n"<<endl;
  }

  if (_calcInt == ActAbsInput::SingleProd) {

    // Set up to calculate single product - option z
     
    ActProdNuclideList getProdNuclides;
    getProdNuclides.storeTable(_prodDataFile.c_str());
 
    cout<<endl;
    cout<<"Enter the single product Z and A values you want to calculate "
	<<"cross-sections and yields for, e.g. 27 60:"<<endl;

    int prodz;
    double proda;
    cin >> prodz >> proda;

    // Load in the full isotope table. Then extract the appropriate
    // isotope and store in the _storeIsotopes object.
      
    // Get the isotope object, containing the half life and decay mode
    // information as well as the Z and A values.
    // This is a new pointer - should not go out of scope when getIsotopes
    // does.
    ActProdNuclide* nuclide = getProdNuclides.getProdNuclide(prodz, proda);
      
    if (nuclide != 0) {
      // Store the new prodNuclide pointer into the _prodNuclideList object.
      // When _prodNuclideList is deleted, the new pointer will also be deleted.
      nuclide->print();
      _prodNuclideList->addProdNuclide(*nuclide);
      
    } else {

      // We can not find this isotope in the decay table. Just add the Z,A nuclide
      // to the _prodNuclideList object with no half life nor side branch information.
      ActProdNuclide* nuclide = new ActProdNuclide(prodz, proda, 0.0);
      nuclide->print();
      _prodNuclideList->addProdNuclide(*nuclide);

    }

  } else if (_calcInt == ActAbsInput::AllProducts) {

    // option a
    // Load in the full isotope table.
    cout<<"Using the complete list of isotope products from "
	<<_prodDataFile<<endl;
    _prodNuclideList->storeTable(_prodDataFile.c_str());
    
  }

}

void ActInput::defineSpectrum() {

  if (_spectrum != 0) {delete _spectrum;}

  cout<<endl;
  cout<<"Choosing cosmic ray spectrum to specify the input beam."<<endl;
  int ZBeam(1);
  double ABeam(1.0);
  _spectrum = new ActCosmicSpectrum("CosmicRays", ZBeam, ABeam);

  cout<<"Enter energies: E(start)  E(end)  bin_width (dE) for beam (all in MeV)"<<endl;
  cout<<"For example: 0.0 1.0e4 100.0"<<endl;
  double EStart, EEnd, dE;
  cin >> EStart >> EEnd >> dE;

  _spectrum->setEnergies(EStart, EEnd, dE);

  cout<<"There are "<<_prodNuclideList->getNProdNuclides()<<" product isotopes\n"<<endl;

}

void ActInput::specifyXSecAlgorithm() {

  if (_xSecAlgorithm != 0) {delete _xSecAlgorithm;}

  cout<<"Using Silberberg and Tsao cross-section algorithm.\n"<<endl;  
  cout<<"Specify the input file containing a list of any (sigma,E) data tables and the"
      <<" minimum allowed value for the data cross-sections, above which formulae are used instead.\n"<<endl;
  cout<<"Type the filename followed by the minimum allowed data cross-section (in mb), or type 0 (zero) "
      <<"for no data tables:"<<endl;
  cout<<"For example: listOfDataFiles.txt 0.001"<<endl;

  std::string dataFileName;
  cin >> dataFileName;

  if (dataFileName.compare("0") == 0) {

    cout<<"Not using any data tables"<<endl;
    _xSecAlgorithm = new ActSTXSecAlgorithm();

  } else {

    double minDataXSec(0.0);
    cin >> minDataXSec;
    cout<<"Using the input file "<<dataFileName<<" for any data tables"<<endl;
    cout<<"Data cross-sections are only used if they are greather than "<<minDataXSec<<" mb, "
	<<"otherwise formulae are used instead.\n"<<endl;

    _xSecAlgorithm = new ActSTXSecAlgorithm(dataFileName.c_str(), minDataXSec);

  }

}

void ActInput::defineTime() {

  cout<<endl;
  cout<<"Exposure period (days) and decay period (days)?"<<endl;
  cout<<"For example: 90.0 180.0"<<endl;

  double texp(0.0), tdec(0.0);
  cin >> texp >> tdec;
  if (_time != 0) {delete _time;}
  _time = new ActTime(texp, tdec);

}

void ActInput::specifyDecayAlgorithm() {

  if (_decayAlgorithm != 0) {delete _decayAlgorithm;}

  cout<<"Using simple exponential radioactive decay algorithm."<<endl;  
  _decayAlgorithm = new ActSimpleDecayAlgorithm(_target, _prodNuclideList, _time);

}

void ActInput::specifyOutput() {

  if (_outputSelection == 0) {return;}

  // Specify default output files etc..
  int defaultType = _outputSelection->getDefaultType();

  if (defaultType == ActOutputSelection::ROOT) {

    // If ROOT is available
    _outputSelection->setXSecFileName("xSecOutput.root");
    _outputSelection->setXSecType(ActOutputSelection::ROOT);
    _outputSelection->setXSecDetail(ActOutputSelection::All);
    _outputSelection->setDecayFileName("decayOutput.root");
    _outputSelection->setDecayType(ActOutputSelection::ROOT);
    _outputSelection->setDecayDetail(ActOutputSelection::All);

  } else {    

    // Otherwise, use ASCII text
    _outputSelection->setXSecFileName("xSecOutput.out");
    _outputSelection->setXSecType(ActOutputSelection::Stream);
    _outputSelection->setXSecDetail(ActOutputSelection::Summary);
    _outputSelection->setDecayFileName("decayOutput.out");
    _outputSelection->setDecayType(ActOutputSelection::Stream);
    _outputSelection->setDecayDetail(ActOutputSelection::Summary);
    
  } 

  cout<<endl;
  cout<<"Finally, specify the output files and their formats.\n"<<endl;

  cout<<"Allowed output format flags are:"<<endl;
  ActOutputSelection::ActOutputMap outputFormats = _outputSelection->getAllowedTypes();

  ActOutputSelection::ActOutputMap::iterator iter;
  int line(0);
  for (iter = outputFormats.begin(); iter != outputFormats.end(); ++iter) {
    int typeFlag = iter->first;
    std::string typeName = iter->second;
    if (line > 0) {cout<<", ";}
    cout<<typeFlag<<" for "<<typeName<<" output files";
    line++;
  }
  cout<<endl;

  cout<<"Allowed level of output detail flags are:"<<endl;

  ActOutputSelection::ActOutputMap outputDetails = _outputSelection->getAllowedDetails();
  line = 0;
  for (iter = outputDetails.begin(); iter != outputDetails.end(); ++iter) {
    int detailFlag = iter->first;
    std::string detailName = iter->second;
    if (line > 0) {cout<<", ";}
    cout<<detailFlag<<" for "<<detailName<<" information";
    line++;
  }
  cout<<endl;

  // Ask the user to select the cross-section output file details
  std::string xSecFileName = _outputSelection->getXSecFileName();
  int xSecType = _outputSelection->getXSecType();
  int xSecDetail = _outputSelection->getXSecDetail();
  cout<<"\nDefault cross-section output fileName, format type and level of detail are:"<<endl;
  cout<<xSecFileName<<"  "<<xSecType<<"  "<<xSecDetail<<endl;
  cout<<"Enter 0 to accept these, or type in all of the new values as in the previous line, "
      <<"separated by spaces:"<<endl;

  std::string tmpString;
  cin >> tmpString;

  if (tmpString != "0") {

    cin >> xSecType >> xSecDetail;

    xSecFileName = tmpString;

    _outputSelection->setXSecFileName(xSecFileName);
    _outputSelection->setXSecType(xSecType);
    _outputSelection->setXSecDetail(xSecDetail);

    cout<<"New cross-section output file settings are: "<<xSecFileName
	<<"  "<<_outputSelection->getXSecType()
	<<"  "<<_outputSelection->getXSecDetail()<<endl;

  } else {
    cout<<"Leaving cross-section output filename, format and level of detail unchanged"<<endl;
  }


  // Ask the user to select the decay yield output file details
  std::string decayFileName = _outputSelection->getDecayFileName();
  int decayType = _outputSelection->getDecayType();
  int decayDetail = _outputSelection->getDecayDetail();
  cout<<"\nDefault decay yield output fileName, format type and level of detail are:"<<endl;
  cout<<decayFileName<<"  "<<decayType<<"  "<<decayDetail<<endl;
  cout<<"Enter 0 to accept these, or type in all of the new values as in the previous line, "
      <<"separated by spaces:"<<endl;

  cin >> tmpString;

  if (tmpString != "0") {

    cin >> decayType >> decayDetail;

    decayFileName = tmpString;

    _outputSelection->setDecayFileName(decayFileName);
    _outputSelection->setDecayType(decayType);
    _outputSelection->setDecayDetail(decayDetail);

    cout<<"New decay output file settings are: "<<decayFileName
	<<"  "<<_outputSelection->getDecayType()
	<<"  "<<_outputSelection->getDecayDetail()<<endl;

  } else {
    cout<<"Leaving decay yield output filename, format and level of detail unchanged"<<endl;
  }

}

void ActInput::printOptions(std::ofstream& stream) {

  stream << "Options are z for one product and a for all active nuclides" << endl;

}

void ActInput::printIntro() {

  cout << endl
       << "Activia - Calculation of Isotope Production Cross-Sections and Yields" << endl
       << "Developed by John Back and Yorck Ramachers" << endl
       << "Copyright (C) 2007-2014 University of Warwick"   << endl << endl;

}
