#!/usr/bin/perl

# Command line for compiling the robot code.
$compile = "gcc -Wall -o robot robot.c -lm";

# Root directory where all code etc is stored.
$rootdir = "/home/web/HTML/Teaching/Resources/COMS21103/robot";

# Command line for displaying the path nicely.
$makesvg = "$rootdir/citytosvg.pl";

# Command line for calculating the penalties.
$penalty = "$rootdir/calculatepenalty.pl";

# Command line for rebuilding the leaderboard.
$leaderboard = "$rootdir/buildleaderboard.pl";

# Timeout in seconds.
$timeout = "31";

# Where the tests are stored.
$testdir = "$rootdir/tests";

# List of test files.

$testlist = "$rootdir/testlist";

# Where the results will be written.
$outputbase = "$rootdir/output/";

# The local folder where the test results are created.
$localdir = "robot-tests";

# Temporary filenames to store test input and results.
$test_input = "$localdir/robot-input.tmp";
$test_timing = "$localdir/robot-results.tmp";

##############################

$thisuser = `whoami`;
chomp $thisuser;

print "Compiling robot.c...\n";
$compile_output = `$compile`;
if ($? != 0) {
    die "Compilation failed";
}

mkdir $localdir;
$outputdir = $outputbase . $thisuser;
chomp $outputdir;
mkdir $outputdir;
chmod 0777, $outputdir;

print "Reading cities...\n";

open TESTFILES, $testlist or die "Couldn't read list of cities";
@testfiles = <TESTFILES>;
close TESTFILES;

chomp @testfiles;

# For each file in the list, run the algorithm with a timeout.
foreach $file (@testfiles) {
    if ($file eq "") {
	next;
    }
    `cp $testdir/$file $test_input`;
    if ($? != 0) {
	die "Couldn't create temporary file";
    }
    print "Testing '$file': ";
    $cmdline = "(time timeout $timeout ./robot $test_input) 2>$test_timing";
    $test_output = `$cmdline`;
    if (length($test_output) > 1000000) {
	print "deleting excessively long output; ";
	$test_output = "";
    }
    print "Output: $test_output\n";
    $secs = 0;
    $msecs = 0;
    open(TIMING,$test_timing) or die "Couldn't read timing output";
    while (<TIMING>) {
	if (/real\s+\d+m(\d+).(\d+)/) {
	    $secs = $1;
	    $msecs = $2;
	}
    }
    close TIMING;
    $time = ($secs*1000)+$msecs;
    print "completed in $time msecs\n";
    
    $pathfile = "$outputdir/$file.path";
    open(OUTPUT,">$pathfile");
    print OUTPUT "$time $test_output\n";
    close OUTPUT;
    chmod 0666, $pathfile;

    # Create a picture displaying the path through the city.
    print "Creating image... ";
    `$makesvg $test_input $outputdir/$file.path >$outputdir/$file.svg`;
    chmod 0666, "$outputdir/$file.svg";
    print "done\n";
}

unlink $test_input, $test_timing;

# Calculate what score should be allocated.
print "Calculating overall score for $thisuser... ";
`$penalty $thisuser`;
print "done\n";

print "Updating leaderboard... ";
`$leaderboard`;
print "done.\n";
