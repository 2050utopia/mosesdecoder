#!/usr/bin/env perl

use strict;

use Getopt::Long;
use File::Basename;
use FindBin qw($RealBin);

sub systemCheck($);

my $mosesDir = "$RealBin/../..";
my $ptPath;
my $lexRoPath;
my $outPath;
my $numLexScores;
my $pruneNum = 0;
my $scfg = 0;

GetOptions("phrase-table=s"  => \$ptPath,
           "lex-ro=s"   => \$lexRoPath,
           "output-dir=s" => \$outPath,
           "num-lex-scores=i" => \$numLexScores,
           "prune=i" => \$pruneNum,
           "scfg" => \$scfg
	   ) or exit 1;

#print STDERR "scfg=$scfg \n";
die("ERROR: please set --phrase-table") unless defined($ptPath);
#die("ERROR: please set --lex-ro") unless defined($lexRoPath);
die("ERROR: please set --output-dir") unless defined($outPath);
#die("ERROR: please set --num-lex-scores") unless defined($numLexScores);

my $cmd;

my $tempPath = dirname($outPath)  ."/tmp.$$";
`mkdir -p $tempPath`;

$cmd = "gzip -dc $ptPath |  $mosesDir/contrib/sigtest-filter/filter-pt -n $pruneNum | gzip -c > $tempPath/pt.gz";
systemCheck($cmd);

if (defined($lexRoPath)) {
  die("ERROR: please set --num-lex-scores") unless defined($numLexScores);                                            

  $cmd = "$mosesDir/bin/processLexicalTableMin  -in $lexRoPath -out $tempPath/lex-ro -T . -threads all";
  systemCheck($cmd);

  $cmd = "$mosesDir/bin/addLexROtoPT $tempPath/pt.gz $tempPath/lex-ro.minlexr  | gzip -c > $tempPath/pt.withLexRO.gz";
  systemCheck($cmd);

  $cmd = "ln -s pt.withLexRO.gz $tempPath/pt.txt.gz";
  systemCheck($cmd);
}
else {
    $cmd = "ln -s pt.gz $tempPath/pt.txt.gz";
    systemCheck($cmd);
}

$cmd = "$mosesDir/bin/CreateProbingPT2 --log-prob --input-pt $tempPath/pt.txt.gz --output-dir $outPath";

if (defined($lexRoPath)) {
    $cmd .= " --num-lex-scores $numLexScores";
}

if ($scfg) {
    $cmd .= " --scfg";
}

systemCheck($cmd);

exit(0);

#####################################################
sub systemCheck($)
{
  my $cmd = shift;
  print STDERR "Executing: $cmd\n";
  
  my $retVal = system($cmd);
  if ($retVal != 0)
  {
    exit(1);
  }
}