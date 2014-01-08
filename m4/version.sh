#!/bin/sh

revision=

if test -f "m4/fuppes-version"; then
    revision=`cat m4/fuppes-version`

elif test -d ".svn"; then

  revision=`cd "$1" && LC_ALL=C svn info 2> /dev/null | grep Revision | cut -d' ' -f2`
  test $revision || revision=`cd "$1" && grep revision .svn/entries 2>/dev/null | cut -d '"' -f2`
  test $revision || revision=`cd "$1" && sed -n -e '/^dir$/{n;p;q}' .svn/entries 2>/dev/null`
  # The revision is unknown
  test $revision || revision="0"
  revision="0.$revision"

elif test -d ".git" || test -d "../.git"; then

  # If taged git releases are eventually used:
  # Git has no logical revision structure so tag releases
  # revision=`git describe 2> /dev/null`

  # currently git is used for development, therefore a version number of zero will do fine
  # you should not be giving this to anyone and therefore a version number of 0.0 will let them
  # know that it is completely unstable
  tag=`git describe --tags HEAD | sed 's/^v//' | sed 's/-.*$//'`
  branch=`git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/'`
  shortcommit=`git rev-parse --short HEAD`
  commitcount=`git rev-list HEAD | wc -l`
  date=`date +%y.%m`
  test $revision || revision=$tag
  test $revision || revision=$branch
  test $revision || revision="git"
  revision="0.$shortcommit-$revision"

else
    revision="0.invalid"  
fi

printf "$revision"
