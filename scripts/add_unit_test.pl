#!/usr/bin/perl -w

###########################################################################
#   Copyright © 2010 Martin Engelmann <murphi.oss@googlemail.com>         #
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
###########################################################################

use strict;
use Getopt::Long;
use Time::localtime;

# Defaults
my $repo= ".";
my $realname = undef;
my $email = undef;
my $testname = undef;
my $testAll = undef;
my $testGetterSetter = undef;
my $testCopy = undef;


GetOptions( "repo=s" => \$repo,
            "realname=s" => \$realname,
            "email=s" => \$email,
            "testname=s" => \$testname,
            "all-tests" => \$testAll,
            "test-getter-setter" => \$testGetterSetter,
            "test-copy" => \$testCopy
            );

if (defined $testAll) {
    $testGetterSetter = 1;
    $testCopy = 1;
}

if (-d "$repo/.git") {
    $realname = `git config user.name` unless defined $realname;
    $email = `git config user.email` unless defined $email;
}
chomp $realname;
chomp $email;

sub isRepo {
    my $repo = shift;
    return -f "$repo/README" && -f "$repo/CMakeLists.txt" && -f "$repo/ChangeLog";
}

die "$repo does not seem to be the root folder of the repo." unless isRepo $repo;
die "Testname not defined" unless defined $testname;
die "Couldn't determine your name." unless defined $realname;
die "Couldn't determine your email." unless defined $email;


my $year = localtime->year() + 1900;
my $copyright =" *   Copyright © $year $realname <$email>";
$copyright .=  " " x (76 - length($copyright)) . "*";
my $license  = <<"END_LICENSE";
/***************************************************************************
$copyright
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
END_LICENSE
my $includeguard = uc($testname) . "_H";

my $testdir = "$repo/src/tests";
my $headerfile = "$testdir/" . lc($testname) . ".h";
my $headerfile_include = lc($testname) . ".h";
my $cppfile = "$testdir/" .lc($testname) . ".cpp";
my $mocfile = lc($testname) . ".moc";
my $cmakefile = "$repo/src/CMakeLists.txt";
my ($testedClass) = ($testname =~ /Test_(.*)/);
my $testedClassHeaderFile = lc($testedClass) . ".h";

die "$headerfile already exists." if -f $headerfile;
die "$cppfile already exists." if -f $cppfile;



open HEADER, ">$headerfile" or die "Could not open $headerfile: $!";
print HEADER  <<"END_HEADER";
$license

#ifndef $includeguard
#define $includeguard

#include <QtCore/QObject>

class $testname : public QObject
{
    Q_OBJECT;

private slots:
    // Add test functions here
    void testFail();
END_HEADER
print HEADER "    void testGetterSetter();\n" if defined $testGetterSetter;
print HEADER "    void testCopyConstructionAndAssignment();\n" if defined $testCopy;
print HEADER <<"END_HEADER";
};

#endif // $includeguard
END_HEADER
close HEADER;



open CPP, ">$cppfile" or die "Could not open $cppfile: $!";
print CPP <<"END_CPP";
$license

#include \"$headerfile_include\"

#include \"$testedClassHeaderFile\"

#include <QtTest/QTest>

// Add test functions here
void ${testname}::testFail()
{
    QFAIL(\"This test is generated and fails intentionally.\");
}

END_CPP

if (defined $testGetterSetter) {
    print CPP "void ${testname}::testGetterSetter()\n{\n";
    print CPP "    $testedClass a;\n\n";
    print CPP "    // For each member of $testedClass set a value and check it.\n";
    print CPP "    // a.setFoo(bar);\n";
    print CPP "    // QCOMPARE(a.foo(), bar);\n";
    print CPP "}\n\n";
}

if (defined $testCopy) {
    print CPP "void ${testname}::testCopyConstructionAndAssignment()\n{\n";
    print CPP "    $testedClass a;\n";
    print CPP "    // Set a value for each member of $testedClass.\n";
    print CPP "    // a.setFoo(bar);\n\n";
    print CPP "    {\n";
    print CPP "        $testedClass b(a);\n";
    print CPP "        // Check if all members are copied correctly.\n";
    print CPP "        // QCOMPARE(b.foo(), a.foo());\n";
    print CPP "    }\n";
    print CPP "\n";
    print CPP "    {\n";
    print CPP "        $testedClass b;\n";
    print CPP "        b = a;\n";
    print CPP "        // Check if all members are copied correctly.\n";
    print CPP "        // QCOMPARE(b.foo(), a.foo());\n";
    print CPP "    }\n";
    print CPP "}\n\n";
}

print CPP <<"END_CPP";

QTEST_MAIN($testname)

#include \"$mocfile\"
END_CPP
close CPP;


open CMAKE_INPUT, "<$cmakefile" or die "Could not open $cmakefile: $!";;
open CMAKE_OUTPUT, ">$cmakefile.$$" or die "Could not open $cmakefile.$$: $!";
my $seen_unit_tests = 0;
my $written_test = 0;
while (my $line = <CMAKE_INPUT>) {
    if($line =~ /set\s*\(\s*UNIT_TESTS/) {
        $seen_unit_tests = 1;
    }

    if($seen_unit_tests && !$written_test && $line =~ /\)/) {
        print CMAKE_OUTPUT "    " . lc($testname) . "\n";
        $written_test = 1;
    }

    print CMAKE_OUTPUT $line;
}
close CMAKE_INPUT;
close CMAKE_OUTPUT;

rename "$cmakefile.$$", "$cmakefile" or die "Failed to rename $cmakefile.$$: $!";

exit 0;
