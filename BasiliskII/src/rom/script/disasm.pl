#!/bin/env perl
# -*- cperl -*-
if( @ARGV < 1 ) {
	print STDERR "usage: disasm first [end]\n";
	exit(1);
}
our $CMD = "m68k-linux-gnu-objdump";
our @OPT = ("-D", "-b", "binary", "-m68040", "$ENV{HOME}/BasiliskII/quadra630.rom",  "--start-address=$ARGV[0]");
if( $ARGV[1] ) {
	push @OPT, "--stop-address=$ARGV[1]";
  }

open my $PIPE, "-|", $CMD, @OPT, or die;
my @A = 0;
my $call = 0;
while(<$PIPE>) {
  if( $. < 8 ) {
	next;
  }
  chomp;
  my ($line, $code, $asm) = split /\t/;
  my ($t) = $line =~ /(\w+):/;
  $code =~ s/\s+$//;
  if( $asm =~ /lea 0x(\w+),%a(\d)/ ) {
	$A[$2+0] = hex($1);
  }
  if( $code =~ /4dfa 0006/ ) {
	$call = $t;
	next;
  } else {
	if( $call ) {
	  $t = $call;
	  $asm =~ s/jmp %pc\@\(0x(\w+)\)/call 0x$1/;
	  $code = "4dfa 0006 $code";
	}
	$call = 0;
  }
  $asm =~ s/jmp \%fp\@/ret/;
  $asm =~ s/jmp \%a(\d)\@(?!\()/retx \%a$1/;
  $asm =~ s/b(..)[swl]/b$1/;
  $asm =~ s/lea %pc\@\(0x(\w+)\)/lea 0x$1/;
  if( $asm =~ /\%pc\@\(0x(\w+),%a(\w):[lw]\)/ ) {
	my $addr = hex($1) + $A[$2+0];
	$code = sprintf ("\%s[ADDR=\$\%05x]", $code, $addr);
  }
  $t = substr "00000$t", length($t), 5;
  print "$t:\t $asm";
  print "\t" x ((38-length($asm))/8);
  print "; $code\n";
}
close $PIPE or die;
