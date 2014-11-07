#! /usr/bin/perl -w

# Create a script that can run the createTables.pl script on 
# all of the MENDL-2P data files.

$listOfFiles = "listOfFiles.txt";
$outputFile = "convertData.sh";

# List all MENDL data files located in the origTables directory.
$command = "ls origTables/*.ndl > $listOfFiles";
system ($command);

open(INFILE, "$listOfFiles");
open(OUTFILE, ">$outputFile");

# Loop over listOfFiles.txt
while (<INFILE>) {

  chop($_);
  $fileName = $_;

  # Rename the file for use in the Activia code
  $newFileName = $fileName;
  $newFileName =~ s/origTables/ActiviaTables/;
  $newFileName =~ s/.ndl/.dat/;

  print OUTFILE "perl createTables.pl $fileName $newFileName\n";

}

close(OUTFILE);
close(INFILE);

print "Running convertData.sh to convert the MENDL data to Activia data tables...\n";
system("sh convertData.sh");
print "...Done\n";
