#!/usr/bin/env perl

use strict;

sub main()
{
    my $file = "list.txt";
    if (defined($ARGV[0])) {
        $file = $ARGV[0];
        #printf("list file comes from: %s\n", $file);
    } else {
        printf("please specify list file\n");
        return;
    }

    print<<EOL;
{
    "test": [
EOL

    open my $fh, $file or die;
    while (<$fh>) {
        s/[\r\n]//;
        my $fn = $_;
        my $str = sprintf("\"%s\",", $fn);
        printf("        %s\n", $str);
    }
    close $fh;

    print<<EOOL;
    ]
}
EOOL
}

main()
