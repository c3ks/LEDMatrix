#!/usr/bin/perl
###########################################
#
###########################################
use warnings;
use strict;

use Device::SerialPort;

my $pari="";
my $serial = Device::SerialPort->new('/dev/ttyUSB0', undef);
$serial->baudrate(19200);
$serial->stopbits(1);
$serial->databits(8);
#$serial->parity($par);
#$serial->parity_enable(1);

$serial->read_char_time(100);     # don't wait for each character
$serial->read_const_time(100); # 1 second per unfulfilled "read" call


for (my $i=1; $i<=5; $i=$i+1){
  my $raw_header=chr(0);
  #emulate mark on 1. char
    if (ord ($raw_header) % 2) {
      #odd
      $pari = "even";
    } else {
      #even
      $pari = "odd";
    }
  $serial->parity($pari);
  $serial->parity_enable(1);
  sleep(0.5);
  my $count_out = $serial->write($raw_header);
  warn "write failed\n"         unless ($count_out);
  warn "write incomplete\n"     if ( $count_out != length($raw_header) );

  

  print "read file $i\n";
  my $data_file="$i.bin";
  open(DAT, $data_file) || die("Could not open file!");
  my $raw_data;
  read (DAT, $raw_data,1000);

  #senden
  #emulate space parity on 1. char
    if (ord ($raw_data) % 2) {
      #odd
      print("Set to odd");
      $pari = "odd";
    } else {
      #even
      print("Set to even");
      $pari = "even";    
    }
  $serial->parity($pari);
  $serial->parity_enable(1);
  sleep(0.5);

  my $summe=0;
  for (my $j=0; $j<=999; $j=$j+1){
    $summe = $summe + ord(substr($raw_data,$j,1));
  }
  $summe = $summe % 256;
  
  printf("0x%x\n", $summe);
  
  $raw_data=$raw_data . chr($summe);

  $count_out = $serial->write($raw_data);
  warn "write failed\n"         unless ($count_out);
  warn "write incomplete\n"     if ( $count_out != length($raw_data) );

  #empfangen
#  my $InBytes=1;
#  (my $count_in, my $string_in) = $serial->read($InBytes);
#
#  warn "read unsuccessful\n" unless ($count_in == $InBytes);
#  print "$count_in , $string_in \n";
  close (DAT);
  sleep(1);
}

