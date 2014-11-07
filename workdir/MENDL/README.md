This directory contains scripts to download the 
Medium Energy Nuclear Data Library (MENDL) data tables and reformat 
them so that they can be used in Activia (and be easier to read).

The sub-directory origTables contains the file downloadMENDL.sh which 
should be run to download the (incident proton) MENDL-2P data files 
from the OECD Nuclear Energy Agency repository:

```sh
cd workdir/MENDL/origTables
$ sh downloadMENDL.sh
```

It uses wget to retrieve each data file from the website 
http://www.oecd-nea.org/dbforms/data/eva/evatapes/mendl_2p. 
Incident neutron data files can be retrieved from 
http://www.oecd-nea.org/dbforms/data/eva/evatapes/mendl_2.

Once this has been done, run the perl script convertData.pl

```sh
$ perl convertData.pl 
```

to convert all of the MENDL files to a more readable form. 
This conversion script requires the file "listOfFiles.txt" that is a 
list of all of the downloaded files within the origTables sub-directory. 
The script should create files in the ActiviaTables sub-directory that 
can then be used within Activia. By default, these new reformatted files 
will have the .dat file extension, but this can be changed in the perl 
script if required. Alternatively, individual MENDL data files can be 
reformatted by using the perl script createTables.pl, which is used
by convertData.pl.

To use the converted datatables when running the program, another 
text file specifying the locations of the ActiviaTables files needs 
to be available.

If using the (recommended) workdir directory, then the 
"listOfDataFiles.txt" template file should already contain the 
expected list of (converted) data files that should be obtained after 
running the "MENDL/origFiles/downloadMENDL.sh" and "MENDL/convertData.pl" 
scripts.

For example, listOfDataFiles.txt should contain lines such as

```
MENDL/ActiviaTables/ag105mp.dat
MENDL/ActiviaTables/ag107mp.dat
etc.
```

When Activia asks for the list of datafiles "(sigma,E) data tables", 
please enter

```
listOfDataFiles.txt minSigma
```

where minSigma is the minimum allowed data cross-section (in mb), 
such as 0.001. The code should then pick up the data files specified 
in the listOfDataFiles.txt file and use these in place of the 
semi-empirical formulae when evaluating cross-sections
(provided the data cross-section is above minSigma).
