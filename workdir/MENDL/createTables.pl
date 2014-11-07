#! /usr/bin/perl -w

$inFile = $ARGV[0];
$outFile = $ARGV[1];

open(INFILE, "$inFile");
open(OUTPUT, ">$outFile");

# Output preamble
print OUTPUT "\# Table format is as follows:\n";
print OUTPUT "\# Target Z A\n";
print OUTPUT "\# New table comment\n";
print OUTPUT "\# Product Z A\n";
print OUTPUT "\# E (MeV)   sigma (mb)\n";

# integer to specify where we are within the data file
$fileSection = 0;

# Material index
$matIndex = 0;
$matZ = 0;
$matA = 0;

# line number
$lineNo = -1;

# line number where the data tables start
$dataLine0 = 0;

# line numbers of start/end of data tables
$readTable = 0;

# line number integer (last integer on each line)
$lastInt = 0;

$codeInt = 0;

# Arrays to store isotope name and index number
@prodZArray = ();
@prodAArray = ();
@prodIndex = ();
$nProducts = 0;
$nTables = 0;

# Array storing each line
@lineArr = ();

# Convert energies from eV to MeV
$EFactor = 1.0e-6;
# Convert cross-sections from barns to mbarns
$SFactor = 1.0e3;

# Create list of elements
&storeElements();

while (<INFILE>) {

  chop($_);
  $line = $_;

  $lineNo++;
  #print "LINENO = $lineNo\n";
  @lineArr = split(/\s+/, $line);

  # Check line; introduce spaces between merged numbers...
  &checkLine();

  # Get material number
  &findMaterial();

  # Find lines of isotope production information
  &findIsotopes();

  # Find the first line number that specifies the
  # number of data points and variables
  &findDataLine();

  # Start reading the data tables
  &readData();

}

close(OUTPUT);
close(INFILE);

if ($nTables != $nProducts) {
  print "Error. Created $nTables tables for $nProducts products\n";
} else {
  print "Successfully created $nTables data tables for $nProducts products\n";
}
print "Finished in createTables.pl\n";

sub checkLine() {

  # Get the last line integer; this is actually the line number in the file
  &getLastInt();

  # Introduce spaces between the codeInt and isotope number.
  # Also introduce space between exponential and material index.

  if ($fileSection eq 4) {

    # Check if the second from last number is 4 characters long.
    # If so, introduce a space between the first and second characters.
    $cntrlInt = $lineArr[$nCols-2];
    $cntrlLength = length($cntrlInt);

    if ($cntrlLength eq 4) {

      # Replace the entries of the line array
      $newInt = substr($cntrlInt, 1, 3);

      @newLineArr = ();
      for ($iCol = 0; $iCol < $nCols; $iCol++) {

	$tmpChar = $lineArr[$iCol];
	$tmpChar =~ s/\ //g;

	if ($iCol == $nCols-2) {
	  push(@newLineArr, $codeInt);
	  push(@newLineArr, $newInt);
	} else {
	  push(@newLineArr, $tmpChar);
	}

      }

      @lineArr = @newLineArr;
      &getLastInt();

    }

    # Now introduce space between exponential and material index
    if ($nCols > 3) {

      $colInt = $nCols-4;
      $cntrlNum = $lineArr[$colInt];
      $newNum = substr($cntrlNum, 0, -4);

      @newLineArr = ();
      for ($iCol = 0; $iCol < $nCols; $iCol++) {

	$tmpChar = $lineArr[$iCol];
	$tmpChar =~ s/\ //g;

	if ($iCol == $colInt) {
	  push(@newLineArr, $newNum);
	  push(@newLineArr, $matIndex);
	} else {
	  push(@newLineArr, $tmpChar);
	}

      }

      @lineArr = @newLineArr;
      &getLastInt();

    }

  }

}

sub findMaterial() {

  if ($lineNo eq 0) {

    $nCols = scalar(@lineArr);
    $matName = $lineArr[1];
    if ($nCols eq 8) {
      $matName = $lineArr[1].$lineArr[2];
    }

    # remove any spaces
    print "Material is $matName\n";
    @matArray = split(/\-/, $matName);
    $matElement = $matArray[0];
    $matZ = getElementZ($matElement);
    $matA = $matArray[1];
    print "Material Z and A values are $matZ and $matA\n";

    print OUTPUT "# Target material is $matName; (Z,A) = ($matZ, $matA)\n";
    print OUTPUT "Target $matZ $matA\n";

  }

  if ($line =~ /MATERIAL\s+(\d+)\s+(\d+)\s+(\d+)/) {
    $matIndex = $1;
    $fileSection = 1;
    print "Material index is $matIndex\n";
  }
}

sub findIsotopes() {

  if ($line =~ /CROSS-SECTION\s+\(\s+(\w+)\s+(\d+)/) {

    $prodZ = getElementZ($1);
    $prodA = $2;

    $index = 0;
    if ($line =~ /MT =\s+(\d+)/ || $line =~ /MT =(\d+)/) {
      $index = $1;
    }

    $fileSection = 2;
    push(@prodZArray, $prodZ);
    push(@prodAArray, $prodA);
    push(@prodIndex, $index);

  } elsif ($line =~ /CROSS-SECTION\s+\(\s+(\w+)/) {

    $isotope = $1;
    @isoArray = split(/\d+/, $isotope);
    $prodZ = getElementZ($isoArray[0]);
    $prodA = $isotope;
    $prodA =~ s/$isoArray[0]//;

    $index = 0;
    if ($line =~ /MT =\s+(\d+)/ || $line =~ /MT =(\d+)/) {
      $index = $1;
    }

    $fileSection = 2;
    push(@prodZArray, $prodZ);
    push(@prodAArray, $prodA);
    push(@prodIndex, $index);

  }

  $nProducts = scalar(@prodIndex);

}

sub findDataLine() {

  if ($fileSection eq 2) {

    # Find line that has only three numbers. The third number
    # on the next line is the line number that we must eventually skip to.

    $nNum = scalar(@lineArr);
    if ($nNum eq 4) {
      $dataLine0 = $lineArr[3];
      $fileSection = 3;
      return;
    }

  } elsif ($fileSection eq 3 && $lineNo == $dataLine0+2) {

    # Find the integer that specifies "data lines"
    $codeInt = $lineArr[1];
    $dataLine0 = $lineNo+$nProducts+4;
    $fileSection = 4;
    return;

  }

}

sub readData() {

  if ($fileSection != 4) {return;}

  if ($lineNo >= $dataLine0) {

    $nCols = scalar(@lineArr);

    # Start reading table
    if ($readTable eq 0 && $nCols eq 8) {

      if ($lineArr[4] eq $matIndex) {

	$readTable = 1;
	$nPoints = $lineArr[1];
	$nVars = $lineArr[2]; # should always be 2 variables (energy, sigma)

	if ($nVars != 2) {
	  print "Error. Number of variables is not 2; $line; $matIndex; LineNo = $lineNo\n";
	  exit(1);
	}

	$nVarCounter = 0;
	#print "Number of points is $nPoints, number of variables is $nVars\n";

	# compare the MT integer (2nd from last number) and extract the
	# product name

	$mtInt = $lineArr[$nCols-2];
	$gotProduct = 0;
	$nProducts = scalar(@prodIndex);
	$prodZ = 0; $prodA = 0;
	$iProd = 0;

	for ($iProd = 0; $iProd < $nProducts; $iProd++) {

	  $prodIdx = $prodIndex[$iProd];

	  if ($prodIdx eq $mtInt) {

	    $prodZ = $prodZArray[$iProd];
	    $prodA = $prodAArray[$iProd];
	    $gotProduct = 1;
	    last;
	  }
	}

	if ($gotProduct eq 0) {
	  print "Error. Could not find the product name for MT number $mtInt\n";
	  exit(1);
	}

	#print "Starting new table\n";

	print OUTPUT "\# New table\n";
	print OUTPUT "Product $prodZ $prodA\n";
	#print OUTPUT "$nPoints\n";

	return;
      }
    }

    if ($readTable eq 1) {

      # Have we read in all of the data points?
      if ($nVarCounter eq $nPoints) {
	# Finished reading this table; Skip next few lines on next iteration
	$readTable = 0;
	$dataLine0 = $lineNo + 3;
	$nTables++;
	return;
      }

      # Data tables have either 2, 4, or 6 "columns" (energy, sigma entries)
      $nCols = scalar(@lineArr);

      $nLoop = 0;
      if ($nCols eq 11) {
	$nLoop = 3;
      } elsif ($nCols eq 10) {
	$nLoop = 2;
      } elsif ($nCols eq 8) {
	$nLoop = 1;
      } else {
	print "nCols = $nCols is not 8, 10 or 11 for $line\n";
	return;
      }

      $iLoop = 0;
      for ($iLoop = 0; $iLoop < $nLoop; $iLoop++) {
	
	$EInt = 2*$iLoop+1;
	$sigmaInt = $EInt+1;

	$energy = $lineArr[$EInt]*$EFactor;
	$sigma = $lineArr[$sigmaInt]*$SFactor;
	
	print OUTPUT "$energy    $sigma\n";

	$nVarCounter += 1;

      }

    }

  }

}

sub getLastInt() {

  $nCols = scalar(@lineArr);
  $lastInt = $lineArr[$nCols-1];

}

sub storeElements() {

  @elemIdxArr = ();
  @elemNameArr = ();

  open(ELEMFILE, "listOfElements.dat");
  while (<ELEMFILE>) {

    chop($_);
    $line = $_;

    @elemLines = split(/\s+/, $line);
    $elemNumber = $elemLines[0];
    $elemName = $elemLines[1];
    # Capitalise the element name
    $elemName = uc($elemName);
    push(@elemIdxArr, $elemNumber);
    push(@elemNameArr, $elemName);

  }

}

sub getElementZ() {

  # Arguments passed to subroutine is in the special array @_.
  # Access elements of this array using $_[n], where n = index
  my $elementName = $_[0];

  my $gotElement = 0;
  $nElements = scalar(@elemNameArr);
  my $ZValue = 0;

  for ($iElem = 0; $iElem < $nElements; $iElem++) {

    my $name = $elemNameArr[$iElem];
    if ($name eq $elementName) {
      $ZValue = $elemIdxArr[$iElem];
      last;
    }
  }

  return $ZValue;

}
