#!/bin/bash

OS=`uname`;
echo "OS:$OS";
if [ "$OS" = "Linux" ]; then
  echo "Linux";
  make clean ;
  ./configure ;
  make ;
  cp src/rogue .
elif [ "$OS" = "Darwin" ]; then
  echo "Darwin" ;
  make clean ;
  ./configure CFLAGS="-g -O2 -w -Wno-return-type" LDFLAGS="-lcurses -liconv";
  make ;
  cp src/rogue .;

  #EUC
  #cp msg/mesg .;
  #./rogue mesg ;

  #UTF-8
  cp msg/mesg.utf-8 . ;
  ./rogue mesg.utf-8 ;
fi
exit ;
