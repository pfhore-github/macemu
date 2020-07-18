#!/usr/bin/perl
# -*- cperl -*-
# ROM DATA DUMPER (Quadra630)
use strict;
our $FILE = $ARGV[0] || "/home/michiaki/BasiliskII/quadra650.rom";
open ROM, $FILE or die;
binmode ROM;
our (%MB, %VIA);
sub readn {
  my ($sz, $ptn, $sk) = @_;
  my $line;
  if( $sk ) {
	seek ROM, $sk, 0;
  }
  read ROM, $line, $sz;
  return unpack $ptn, $line;
}
sub read4 {
  my ($sk) = @_;
  return readn(4, "l>", $sk);
}
sub read2 {
  my ($sk) = @_;
  return readn(2, "s>", $sk);
}
sub read1 {
  my ($sk) = @_;
  return readn(1, "c", $sk);
}
sub put_if {
  my $mask = 0xfffff;
  if( $_[0] == 0 ) {
	return "-";
  } else {
	return sprintf '$%05X', $_[0] & $mask;
  }
}
sub split_bit {
  my ($v, $n) = @_;
  return map { ($v >> ($n-$_-1)) & 1; } ( 0 .. $n-1);
}
#our $TABLE1 = 0x3260;
#our $TABLE2 = 0xD2800;

sub dump_model {
  my $addr = $_[0];
  seek ROM, $addr, 0;
  my $x;
  read ROM, $x, 62;
  my @v = unpack "N4nC2nC2N9n", $x;
  $MB{($v[0]+$addr)&0xffffffff}=1;
  $VIA{($v[13]+$addr)&0xffffffff}=1;
  $VIA{($v[14]+$addr)&0xffffffff}=1 if $v[14] != 0;
  for( @v[0,1,2,3,14 .. 18]) {
	$_ = put_if($_ ? $addr + $_ : 0);
  }
  print 
	join(",",
		 sprintf ('$%05X', $addr),
		 @v[0..3],
		 split_bit($v[4],16),
		 sprintf('$%02X', $v[5]),
		 sprintf('$%02X', $v[6]),
		 sprintf('$%04X', $v[7]),
		 $v[8], $v[9],
		 split_bit($v[10],32),
		 split_bit($v[11],32),
		 sprintf ('$%08X', $v[12]),
		 sprintf ('$%08X', $v[13]),
		 @v[14..18],
		 sprintf ('$%04X', $v[19])
		), "\n";
  
}

sub dump_model_table {
  my $offset;
  my @models;
  for(my $base =$_[0];
	  ( $offset = read4($base)) != 0;
	  $base += 4) {
	printf '$%05X,', $base;
	dump_model($base+$offset);
  }
  return @models;
}

#dump_model_table($TABLE2);
#dump_model_table($TABLE1);


sub dump_mb {
#  for( sort keys %MB ) {
  for( ( 0xD3034 ) ) {
	my $addr = $_;
	seek ROM, $addr-20, 0;
	my $x;
	read ROM, $x, 140;
	my @v = unpack "N2C4NCx3N34", $x;
	for( @v[8 .. 36]) {
	  $_ = $_ ? sprintf '$%08X', $_ : '-';
	}
	print 
	  join(",",
		   sprintf ('$%05X', $addr),
		 split_bit($v[0],32),
		 split_bit($v[1],32),
		 sprintf('$%02X', $v[2]),
		 sprintf('$%02X', $v[3]),
		 sprintf('$%02X', $v[4]),
		 sprintf('$%02X', $v[5]),
		 sprintf('$%04X', ($addr+$v[6])&0xffff),
		 sprintf('$%02X', $v[7]),
		 @v[8..36],
		 sprintf ('$%08X', $v[37]),
		), "\n";
  }
}
dump_mb();

sub dump_via_data {
  for( sort keys %VIA ) {
	my $addr = $_;
	seek ROM, $addr, 0;
	my $x;
	read ROM, $x, 6;
	my @v = map { sprintf '$%02X', $_ } unpack "C6", $x;
	print +(sprintf '$%05X,', $_),
	  join (",", @v), "\n";
  }
}
#dump_via_data();
sub dump_jmp_table {
  my $a1 = 0x3240;
  printf "%06x\n", $a1;
  for(1 .. 7 ) {
	my $a0 = $a1 + read4($a1);
	my $a2 = read4($a0-8);
	printf '$%05X,$%05X,$%05X' . "\n",
	  $a1, $a0, $a2+$a0;
	$a1 += 4;
  }
}
#dump_jmp_table();
sub dump_49bc_4a28 {
  my $d0 = read2(0x49bc+($_[0])*2) & 0xffff;
  my $d1 = read2(0x4a28+($_[0])*2) & 0xffff;
  my $d3 = read1(0x4a30+($_[0])) & 0xff;
  my $d4 = read2(0x4a34+($_[0])*2) & 0xffff;
  my $fmt = <<'END';
%d,$%04X,$%04X,$%04X,$%04X,$%04X,$%02X,$%04X,$%04X
END
  printf $fmt, $_[0], 0x49bc+($_[0])*2, $d0, 0x4a28+($_[0])*2, $d1,
	0x4a30+($_[0]), $d3, 0x4a34+($_[0])*2, $d4;
}

sub dump_49bc_4a28 {
  my $d0 = read2(0x4a20+($_[0])*2) & 0xffff;
  my $d1 = read2(0x4a28+($_[0])*2) & 0xffff;
  my $d3 = read1(0x4a30+($_[0])) & 0xff;
  my $d4 = read2(0x4a34+($_[0])*2) & 0xffff;
  my $fmt = <<'END';
%d,$%04X,$%04X,$%04X,$%04X,$%04X,$%02X,$%04X,$%04X
END
  printf $fmt, $_[0], 0x4a20+($_[0])*2, $d0, 0x4a28+($_[0])*2, $d1,
	0x4a30+($_[0]), $d3, 0x4a34+($_[0])*2, $d4;
}

#for my $id (0 .. 3 ) {
#  dump_4a20_4a28($id);
#  dump_49bc_4a28($id);
#}
