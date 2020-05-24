#!/usr/bin/perl
# -*- cperl -*-
# ROM DATA DUMPER (Quadra630)
use feature ':all';
use strict;
open ROM, "/home/michiaki/BasiliskII/quadra630.rom" or die;
binmode ROM;
sub read_v {
  my ($p, $t, $sz) = @_;
  my $v;
  seek ROM, $p, 0;
  read ROM, $v, $sz;
  my ($x) = unpack "$t>", $v;
  return $x;
}
sub read_4 { return read_v($_[0], "l", 4); }
sub read_2 { return read_v($_[0], "s", 2); }
sub read_1 { return read_v($_[0], "c", 1); }
for(my $b = 0x3260; $b < 0x3294; $b += 4 ) {
  my $off = $b + read_4($b);
  my $id = read_2($off+68);
  printf "\$%05X\t\$%04X\n", $b, $id;
}
