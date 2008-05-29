#!/usr/bin/perl

use strict;

use lib;
use File::Basename;

BEGIN {
  push @INC, dirname($0);
}

use urpm;
use urpm::media;
use urpm::args;

use urpmi_backend::open_db;
use urpmi_backend::tools;
use urpmi_backend::filters;

use perl_packagekit::enums;
use perl_packagekit::prints;

# Two arguments (filter, search term)
exit if ($#ARGV != 1);

my @filters = split(/;/, $ARGV[0]);
my $search_term = $ARGV[1];

my $basename_option = FILTER_BASENAME;
$basename_option = grep(/$basename_option/, @filters);

my $urpm = urpm->new_parse_cmdline;

urpm::media::configure($urpm);

my $db = open_rpm_db();
$urpm->compute_installed_flags($db);

# Here we display installed packages
if(not grep(/^${\FILTER_NOT_INSTALLED}$/, @filters)) {
  $db->traverse(sub {
      my ($pkg) = @_;
      if(filter($pkg, \@filters, {FILTER_DEVELOPMENT => 1, FILTER_GUI => 1})) {
        if( (!$basename_option && $pkg->name =~ /$search_term/)
          || $pkg->name =~ /^$search_term$/ ) {
          pk_print_package(INFO_INSTALLED, get_package_id($pkg), ensure_utf8($pkg->summary));
        }
      }
    });
}

# Here are packages which can be installed
if(grep(/^${\FILTER_INSTALLED}$/, @filters)) {
  exit 0;
}

foreach my $pkg(@{$urpm->{depslist}}) {
  if($pkg->flag_upgrade && filter($pkg, \@filters, {FILTER_DEVELOPMENT => 1, FILTER_GUI => 1})) {
    if( (!$basename_option && $pkg->name =~ /$search_term/)
      || $pkg->name =~ /^$search_term$/ ) {
      pk_print_package(INFO_AVAILABLE, get_package_id($pkg), ensure_utf8($pkg->summary));
    }
  }
}