#! /usr/gnu/bin/perl

# $Header$
#
# For log, see ChangeLog

$startnum = <>;
$startnum =~ s/^[^0-9]*([0-9]*)([^0-9]*)?$/$1/;

while ($curnum = <>) {
    $curnum =~ s/^[^0-9]*([0-9]*)([^0-9]*)?$/$1/;
    if ($curnum != ($startnum + 1)) {
	print STDERR "SKIP: $startnum->$curnum\n";
    }
    $startnum = $curnum;
}
