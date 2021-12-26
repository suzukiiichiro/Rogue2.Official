#!/bin/bash

OS=`uname`;
echo "OS:$OS";
if [ "$OS" = "Linux" ]; then
  echo "Linux";
  /bin/rm -fr rogue;
  make clean ;
  ./configure ;
  make ;
  /bin/cp src/rogue .
elif [ "$OS" = "Darwin" ]; then
  echo "Darwin" ;
  /bin/rm -fr rogue;
  make clean ;
  ./configure CFLAGS="-g -O2 -w -Wno-return-type" LDFLAGS="-lcurses -liconv";
  make ;
  /bin/cp src/rogue .;
fi
./rogue mesg ;
exit ;
