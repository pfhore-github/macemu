#!/usr/bin/perl
# -*- cperl -*-
# ROM DATA DUMPER (Quadra630)
use strict;
our $FILE = $ARGV[0] || "/home/michiaki/BasiliskII/quadra630.rom";
open DUMP, ">b99b0.bin" or die;
open ROM, $FILE or die;
binmode ROM;
seek ROM, 0xB99B0, 0;
my $data;
read ROM,$data, 0xB9A18-0xB99B0;
print DUMP $data;
close ROM or die;
close DUMP or die;
