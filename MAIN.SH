#!/bin/bash

OS=`uname`;
echo "OS:$OS";
if [ "$OS" = "Linux" ]; then
  echo "Linux";
  #インストールしていないなら絶対必要
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
  #インストールしていないなら絶対必要
  brew install automake;
  # Macだと完全に動かない環境もたーくさあんあります
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
