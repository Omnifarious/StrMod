#! /usr/gnu/bin/perl

# $Header$
#
# For log, see ChangeLog

$startnum = <>;
$startnum =~ s/^[^0-9]*([0-9]*)([^0-9]*)?$/$1/;

while ($curnum = <>) {
    if ($curnum =~ /^Event/) {
	$curnum =~ s/^[^0-9]*([0-9]*)([^0-9]*)?$/$1/;
	if ($curnum != ($startnum + 1)) {
	    print "ERROR: skip $startnum->$curnum\n";
	}
	$startnum = $curnum;
    } elsif ($curnum =~ /^ Count/) {
	$curnum =~ /^[^0-9]*([0-9]+)[^0-9]+([0-9]+)[^0-9]+([0-9]+)[^0-9]*$/;
	($n1, $n2, $n3) = ($1, $2, $3);
	if ($n1 != ($startnum + 1)) {
	    print "ERROR: Wrong ending value: $n1\n";
	} else {
	    print "OK: $n1 ", $startnum + 1, "\n";
	}
	if (($n1 != $n2) || ($n2 != $n3)) {
	    print "ERROR: Ending numbers don't match: $n1 $n2 $n3\n";
	} else {
	    print "OK: $n1 $n2 $n3\n";
	}
    }
}
