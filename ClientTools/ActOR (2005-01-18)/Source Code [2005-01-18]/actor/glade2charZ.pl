#!/usr/local/bin/perl
use Compress::Zlib;

$VAR_NAME = "windowXMLstring";

my $sourcefile;
my $comp_dest;

#undef $sourcefile;
#undef $comp_dest;
while (<>) {
  $sourcefile .= $_;
}
my $orig_len = length($sourcefile);

$comp_dest = compress($sourcefile);
my $comp_len = length($comp_dest);

print "#define ".uc($VAR_NAME)."_ORIG_LEN $orig_len\n";
print "#define ".uc($VAR_NAME)."_COMP_LEN $comp_len\n";
print "\n";
print "unsigned char *".$VAR_NAME.";\n";
print "static unsigned char ".$VAR_NAME."comp[".$comp_len."] = {\n";

my $count = 0;
while ($count < $comp_len) {
  if (($count % 10) == 0) {
    print "\n";
  }
  printf "0x%02x, ", unpack("C", substr($comp_dest, $count, 1));
  $count += 1;
}

print "\n\n};\n";

exit;

