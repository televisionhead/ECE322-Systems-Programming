#!/bin/csh

if (${#argv} < 1) then
	echo "usage: cr_project <project_name>"
	exit -1
endif

set MKD = /bin/mkdir
set TOUCH = /bin/touch
set PROJNAME = $1
set PROJPATH = $PWD/$PROJNAME

if (-d $PROJPATH) then
	echo "error! that directory already exists"
	exit -1
endif

$MKD "$PROJPATH"
$MKD "$PROJPATH/bin"
$MKD "$PROJPATH/src"
$TOUCH "$PROJPATH/src/main.c"

if (-d $PROJPATH) then
	cp $PWD/$0 $PROJPATH/$0
	echo "your project directory has been created"
	exit 0
else 
	echo "error! mkdir failed to make the project directory"
	exit -1
endif