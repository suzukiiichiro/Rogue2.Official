#!/bin/bash

OS=`uname`;
echo "OS:$OS";
if [ "$OS" = "Linux" ]; then
  echo "Linux";
  #$B%$%s%9%H!<%k$7$F$$$J$$$J$i@dBPI,MW(B
  yum install ncurses-devel
  #
  if [ -f rogue ]; then
    /bin/rm -fr rogue;
  fi
  make clean ;
  ./configure ;
  make ;
  #
  /bin/cp src/rogue .
elif [ "$OS" = "Darwin" ]; then
  echo "Darwin" ;
  #$B%$%s%9%H!<%k$7$F$$$J$$$J$i@dBPI,MW(B
  brew install automake;
  # Mac$B$@$H40A4$KF0$+$J$$4D6-$b$?!<$/$5$"$s$"$j$^$9(B
  #
  #
  if [ -f rogue ]; then
    /bin/rm -fr rogue;
  fi
  make clean ;
  ./configure CFLAGS="-g -O2 -w -Wno-return-type" LDFLAGS="-lcurses -liconv";
  make ;
  /bin/cp src/rogue .;
fi
./rogue mesg ;
exit ;
