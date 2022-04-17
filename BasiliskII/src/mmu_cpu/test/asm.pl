#!/bin/env perl

our %ASM;
for(<*.cpp>) {
    open FILE, $_ or next;
    while(<FILE>) {
        if( /asm_m68k\("(.+?)"\)/ ) {
            $ASM{$1} = "";
        }
    }
}
open OUT, ">_test_common2.cpp" or die;
print OUT <<"PRE";
#include <unordered_map>
#include <vector>
#include <string>
std::unordered_map<std::string, std::vector<int>> asmcodes =
{
PRE
for( keys %ASM) {
    my $tmp = "/tmp/asm_ret_$$.S";
    open my $pipe1, "| m68k-linux-gnu-as -mcpu=68040 -o $tmp" or die;
    print $pipe1 "$_\n" or die;
    close $pipe1 or die;
    my $ret = `m68k-linux-gnu-objcopy -O binary $tmp /dev/stdout`;
    my @c = unpack "n*", $ret;
    for( @c ) { $_ = sprintf "0x%04x", $_; }
    print OUT qq/{ "$_", { /;
    print OUT join(",", @c);
    print OUT "} },\n";
    unlink $tmp or die;
}
print OUT "};\n";
close OUT;
