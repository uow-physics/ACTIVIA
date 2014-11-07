README of the ACTIVIA package
=============================
Contact: John Back (J.J.Back@warwick.ac.uk) and 
	 Yorck Ramachers (Y.A.Ramachers@warwick.ac.uk)

Introduction
------------

This C++ package can calculate target-product cross-sections and the 
production and decay yields of (radioactive) isotopes from cosmic ray 
activation using data tables and semi-empirical formulae, as detailed 
in the paper "ACTIVIA: Calculation of isotope production cross-sections 
and yields", published in Nucl. Instrum. Meth A. 586, 286 (2008), 
also available on the arXiv: http://uk.arxiv.org/abs/0709.3472.

This file explains how to build and run the ACTIVIA code to perform
calculations of cross-sections and yields for radioactive isotopes.
The code has only been tested to work within the Unix environment.

Further online documentation is available at
http://universityofwarwick.github.io/ACTIVIA

[![DOI](https://zenodo.org/badge/doi/10.5281/zenodo.12612.png)]
(http://dx.doi.org/10.5281/zenodo.12612)


Building the code  
-----------------

This package does not require any other external software to run.
However, it can use ROOT, a general purpose C++ data analysis
framework (http://root.cern.ch), for creating output files, and a
graphical user interface (GUI) can be enabled based on the open source
version of the Qt software framework (https://qt-project.org/downloads).

The configure script should be run to set the compiler options,
and to enable the ROOT output and/or the GUI extension.

```sh
$ ./configure
$ make
```

Use "configure --help" to see the list of available options. The
configuration will try to automatically detect if your system has
ROOT installed (version 5.14.00e and above required). Otherwise, use 
the "configure --rootsys" option. The GUI can be configured using 
"configure --gui". This requires a full installation of Qt (version 4);
the "--qtlib", "--qtinc" and "--moc" options should specify the location 
of the Qt libraries, include files and the Meta-Object Compiler (moc)
command, respectively.

By default, the compilation creates a shared library "lib/libActivia.so"
as well as the binary "bin/Activia".

Note that the shared library "lib/libActivia.so" needs to be included in the
LD_LIBRARY_PATH environment variable before "bin/Activia" can be run using
(for csh/tcsh):

setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:base_directory_of_Activia/lib

This can be set using the "pathlib.sh" script from the Activia base
directory. For csh/tcsh:

```sh
$ source pathlib.sh
```

                          
Running the code
----------------

After creating the "bin/Activia" executable, you can run the code by going 
to the workdir directory

```sh
$ cd workdir
```

This is the recommended place to run the code, since this directory contains
(a soft-link to) the necessary "decayData.dat" file, which specifies all 
possible isotope products together with their side-branches. The format 
of each line in "decayData.dat" is

productZ   productA   halfLife   nSideBranches  SBZ1 SBA1 ... SBZn SBAn

where SBZi and SBAi are the Z and A values of side-branch i, respectively. 
The half-life value must be in units of days.


a) The recommended way to run the program is to use an input file.
Examples are provided in the workdir/runFiles sub-directory:

```sh
$ cd workdir
$ sh runFiles/runNatCu_Co60.sh
```


b) To run the code in a terminal window (no GUI), issue the command

```sh
$ ./bin/Activia
```

where the program will ask you a set of questions which you need 
to provide interactively. If a question requires several input 
values, then they all need to be put on the same line, each 
separated by a space, before typing enter (carriage return) to 
accept them. Please do not use other symbols (such as commas) to 
separate the input values as this will result in I/O errors.


c) If you want to use the GUI, make sure the code has been compiled and 
linked with the Qt 4 libraries (see above). Then issue the command

```sh
$ ./bin/Activia 1
```

where a GUI window will pop-up (the number 1 specifies we want to 
use the GUI).

If there is an error saying that the Qt libraries cannot be found, 
then set the environment variable LD_LIBRARY_PATH to include the 
Qt library path:

setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:location_of_Qt_libraries

Right clicking on any section in the window will provide some 
help/hints on how to fill in the form. Clicking on "Run" at the bottom 
will run the code, where another window will pop-up, detailing the 
progress of the calculation. Once the form has been filled in, the 
input can be saved as a file ("File" -> "Save input"). 
Reloading an input file will preset the various entries in the form 
("File"->"Load input").



To run the example of calculating all isotope product yields for
natural tellerium and using ROOT output files, do

```sh
$ cd workdir
$ sh runFiles/runNatTe_All.sh
```

To use ASCII output files, run the following script instead:

```sh
$ sh runFiles/runNatTe_All_ascii.sh
```

The run file specifies all required inputs for the calculations and also 
provides the names and level of detail (summary or full information) 
of the cross-section and yield output files.


Data tables                               
-----------

If you want to use data tables, then the workdir must also contain a file 
(e.g. "listOfDataFiles.txt") that specifies the filenames of all of the 
ASCII tables. This file must be provided interactively or in the run 
script file.

Each data file has a set of initial comments specified by the hash (#) 
symbol, followed by the target Z and A values, the "#New table" line 
and then the product Z and A values. The remaining lines are the actual 
energy and cross-section data for the given target-product pair. 
Another set of product cross-section values can be provided after the 
lines "#New table" and "Product ZProd AProd" for the 
same target isotope (specified at the start of the file).

The directory workdir/MENDL contains a README.md file with instructions on 
downloading Medium Energy Nuclear Data Library (MENDL) data files from the 
OECD Nuclear Energy Agency repository that can then be used in Activia. 
The template file workdir/listOfDataFiles.txt should already contain a list 
of the MENDL data files, provided the steps in workdir/MENDL/README.md are 
followed.

An example script of running Activia with the MENDL data tables can be seen
in the file

```sh
$ sh runFiles/runNatTe_All_DataTables.sh
```

which contains the line

```
listOfDataFiles.txt 0.001
```

This tells Activia to read any data files specified in listOfDataFiles.txt,
and the number 0.001 is the minimum cross-section (in mb) for the data, i.e.
if the data value is below this number, then the formulae are used instead.


It is also possible to use data files that are not based on the MENDL tables.
As an illustrative example, consider a file myDataFiles.txt that lists
only two filenames for cross-section data for two target isotopes; one for 
the target isotope As-74 and another one for the target isotope Ba-133:

as74.dat
ba133.dat

The file "as74.dat" contains the following energy-dependent
cross-section data for three example products, Se-74, Se-73 and Se-72:

```
# Table format is as follows:
# Target Z A
# New table comment
# Product Z A
# E (MeV)   sigma (mb)
# Target material is AS-74; (Z,A) = (33, 74)
Target 33 74
# New table
Product 34 74
1e-11    0
2    0.217
3    8.32
4    55.8
.
.
(provide all necessary energy and cross-section data)
.
.
# New table
Product 34 73
11.655    0
13    25.2
14    102
15    208
.
.
.
# New table
Product 34 72
20.158    0
21    0.0626
22    7.57
23    53.6
.
.
.
```

The file "ba133.dat" will need to have a similar structure.

                          
Output variables
----------------

The code produces two output files: one for cross-section data and 
production rates, another one for decay yield information. Below we
describe the output variables in the ROOT files. Variables in the ASCII
(Stream) output files have the same meaning.

In the cross-section output file, we have two TTree's named "xSecEData",
which stores detailed cross-section vs energy results, and "xSecSummary",
which provides summary information.

Variables in "xSecEData" are:

```
Zt       = target isotope Z number
At       = target isotope A number
frac     = relative abundance fraction of the target isotope (0.0 to 1.0)
Z        = product isotope Z number
A        = product isotope A number
nPoints  = number of energy points or bins (used in the beam spectrum)
Energy   = energy of the beam (MeV)
Sigma    = cross-section (mb), not weighted by the relative target isotope
           fraction, frac
ProdRate = production rate (per kg per day), weighted by the relative
           target isotope fraction, frac
```

Variables in "xSecSummary" are:

```
ProdZ       = product isotope Z number
ProdA       = product isotope A number
TotSigma    = total cross-section for the product isotope (mb), summed over
	      all energy bins and normalised over all target isotope 
              fractions
TotProdRate = total production rate (per kg per day), normalised 
              over all target isotope abundance fractions.
```

The decay yield output file has two TTree's, one named "initialYields", 
containing information about the initial production rate for product 
isotopes and their side-branches, and another one named "decaySummary", 
which summaries the initial and final yields of all product isotopes, 
weighted over all target isotope abundance fractions.

Variables in "initialYields" are:

```
Zt    = target isotope Z number
At    = target isotope A number
Z     = product isotope Z number
A     = product isotope A number
SB    = side-branch flag (1 = yes, 0 = no)
R     = production rate of isotope/side-branch (per kg per day)

R_tot = production rate of isotope (Z,A), summing over side-branches 
         (per kg per day)
      = 0.0 for side-branches

I0    = initial product yield rate = R_tot*(1.0 - exp(-lambda*t_exp)), where
        lambda = ln2/t_Half,
        t_Half = half-life of the product isotope (days) and
        t_exp is the beam exposure time (days).
```

Variables in "decaySummary" are:

```
ip    = product isotope order number
Z     = product isotope Z number
A     = product isotope A number
tHalf = half-life of the product isotope (days), specified in the 
        "decayData.dat" file
dndti = initial yield rate (per kg per day), at the start of the cooling 
        (beam off) period, weighted over all target isotopes
dndtf = final yield rate (per kg per day), weighted over all target isotopes
```

Validation QA
-------------

The sub-directory workdir/validation contains the runComparisons.sh script:

```sh
$ sh runComparisons.sh
```

which can be run to compare various experimental measurements 
(from the EXFOR database https://www-nds.iaea.org/exfor/exfor.htm) 
with the calculations from Activia for a range of target materials 
and activation products. Before this script can be run, the 
"workdir/validation/analysis" program needs to be built using "make", 
which requires ROOT. The comparison script creates a series of plots 
in the workdir/validation/pngFiles directory.

Note that some of the comparisons require the MENDL-2 data files to be present
in workdir/MENDL (specifically the last four examples in runComparisons.sh).

Specific reference plots are available in the "Activia/doc/pngFiles" directory,
and can be viewed within the automatic Doxygen documentation (see below).


Detailed C++ code documentation
-------------------------------

All of the C++ classes have Doxygen (http://www.doxygen.org)
comments provided. If the "Activia/doxygen" sub-directory does not exist,
then the documentation can be made by doing (providing Doxygen is 
installed on your system):

```sh
$ doxygen
```

This will create a "Activia/doxygen" subdirectory which contains the Doxygen 
documentation. Load the page "doxygen/html/index.html" in a web browser to
look through the documentation and class diagrams. The Unix program "dot" 
is used to create the class diagrams and needs to be installed on your 
system (it is part of Graphviz tool). Otherwise, set HAVE_DOT to NO in the 
Doxyfile.


License
-------

This software is distributed under the Boost Software License, Version 1,
(Aug 17 2003). See [LICENSE_1_0.txt](LICENSE_1_0.txt)
(optionally the original at http://www.boost.org/LICENSE_1_0.txt)
for details.


Authors and contributors
------------------------

The authors are John Back and Yorck Ramachers.
