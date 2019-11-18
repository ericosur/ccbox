#!/usr/bin/env perl

use strict;

my $app = '/home/rasmus/src/ccbox/opencv/capture/build/fixdep';
my @arr = glob('*.bin');

for my $inf (@arr) {
    my $otf = $inf;
    if ( $inf =~ m/depth(\d+)\.bin/ ) {
        my $n = $1;
        $n = $n % 100000;
        $otf = sprintf "fixdep%d.png", $n;
    }
    #printf STDOUT "%s => %s\n", $inf, $otf;
    my $cmd = sprintf "%s %s %s", $app, $inf, $otf;
    system $cmd;
}
