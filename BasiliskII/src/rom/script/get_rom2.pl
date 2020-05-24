#!/usr/bin/perl
# -*- cperl -*-
# ROM DATA DUMPER (Quadra650)
use strict;
open ROM, "/home/michiaki/BasiliskII/quadra630.rom" or die;
binmode ROM;
for( my $i = 0; $i < 45; ++$i ) {
  seek ROM, 0xB97F2 + $i*4, 0;
  my $offset_b;
  read ROM, $offset_b, 4;
  my ($offset) = unpack "l>", $offset_b;
  seek ROM, 0xB97F2+$offset, 0;
  printf "%d,\$%08X,\$%08X\n", $i, ($offset&0xffffffff), (0xB97F2+$offset);
}
