This directory contains the runComparisons.sh script:

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

Specific reference plots are available in the "Activia/doc/pngFiles" directory,
and can be viewed within the automatic Doxygen documentation.


