#!/bin/env perl
my $X;
for my $file (<*.cpp>) {
  open FILE, "<$file" or next;
  while(<FILE>) {
	if ( /void run_(.....)\(\) {/ ) { $X{$1}=1; }
  }
  close FILE;
}
open FILE, ">prototype2.hpp";
for( sort keys %X) {
  print FILE "void run_$_() ;\n";
}
