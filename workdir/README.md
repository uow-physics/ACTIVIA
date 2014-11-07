This is the recommended place to run the code, since this directory 
contains (a soft-link to) the necessary decayData.dat file, which 
specifies all possible isotope products together with their side-branches. 
The format of each line in decayData.dat is

productZ productA halfLife nSideBranches SBZ1 SBA1 ... SBZn SBAn

where SBZi and SBAi are the Z and A values of side-branch i, respectively. 
The half-life value must be in units of days.

a) The recommended way to run the program is to use an input file. 
Examples are provided in the runFiles sub-directory:

```sh
$ sh runFiles/runNatCu_Co60.sh
```

b) To run the code in a terminal window (no GUI), issue the command

```sh
$ ./bin/Activia
```

where the program will ask you a set of questions which you need to 
provide interactively. If a question requires several input values, 
then they all need to be put on the same line, each separated by a 
space, before typing enter (carriage return) to accept them. 
Please do not use other symbols (such as commas) to separate the 
input values as this will result in I/O errors.

c) If you want to use the GUI, make sure the code has been compiled 
and linked with the Qt 4 libraries (see above). Then issue the command

```sh
$ ./bin/Activia 1
```

where a GUI window will pop-up (the number 1 specifies we want to use the GUI).

If there is an error saying that the Qt libraries cannot be found, 
then set the environment variable LD_LIBRARY_PATH to include the Qt library path:

```sh
$ setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:location_of_Qt_libraries
```

Right clicking on any section in the window will provide some help/hints 
on how to fill in the form. Clicking on "Run" at the bottom will run the code, 
where another window will pop-up, detailing the progress of the calculation. 
Once the form has been filled in, the input can be saved as a file 
("File" -> "Save input"). Reloading an input file will preset the various 
entries in the form ("File"->"Load input").

To run the example of calculating all isotope product yields for natural 
tellerium and using ROOT output files, do

```sh
$ sh runFiles/runNatTe_All.sh
```

To use ASCII output files, run the following script instead:

```sh
$ sh runFiles/runNatTe_All_ascii.sh
```

The run file specifies all required inputs for the calculations and 
also provides the names and level of detail (summary or full information) 
of the cross-section and yield output files.

-------------------------------------------------------------------------

To include data tables for the cross-section calculations, please
see the instructions in workdir/MENDL/README.md.

-------------------------------------------------------------------------

The sub-directory validation contains the runComparisons.sh script:

```sh
$ sh runComparisons.sh
```

which can be run (after building the "analysis" program in the same directory
using "make") to compare various experimental measurements
(from the EXFOR database https://www-nds.iaea.org/exfor/exfor.htm) with the
calculations from Activia for a range of target materials and activation
products. This comparison creates a series of plots in the 
workdir/validation/pngFiles directory using ROOT.

Note that some of the comparisons require the MENDL-2 data files to be present
in workdir/MENDL (specifically the last four examples in runComparisons.sh).

