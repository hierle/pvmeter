#!/usr/bin/perl -w

# simple pvmeter integration
use English;
use strict;
use Socket;

my $nowint = time;
my $nowstr = int2time($nowint);
my $today = `date +%Y%m%d`;
chomp $today;
my $reqm = $ENV{'REQUEST_METHOD'};
#if ((!defined $reqm)||($reqm ne "GET")) { response(400,"illegal request method"); exit; }
my $logfile = "/srv/apache/public_html/data/$today.txt";

if ($reqm eq "POST") {
 # write log file
 open FH, ">> $logfile";
 while (<STDIN>) { print FH $_ . "\n"; }
 close FH;
 
 response(200,"OK");
}
else {
 my $content = `cat $logfile`;
 response(200,$content);
}

exit;

##############
sub response {
  my $status = shift;
  my $message = shift;

  my $html = "Status: $status
Date: $nowstr
Last-modified: $nowstr
Pragma: no-cache
Cache-control: no-cache
Content-Type: text/plain; charset=utf-8

$message
\n";
  print $html;
}

##############
sub int2time {
    my $nowint=$ARG[0];
    if (!defined $nowint) { $nowint = time; }
    my $nowstr;
    $nowstr = gmtime($nowint);
    if ( $nowstr =~ /^(...)\ (...)\ (..)\ (..:..:..)\ (....)$/ ) {
         $nowstr = "$1\, $3 $2 $5 $4 GMT";
    }
    return $nowstr;
}
