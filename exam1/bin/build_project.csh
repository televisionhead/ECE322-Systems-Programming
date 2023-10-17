#!/bin/csh

if (${#argv} < 1) then
	echo "usage: build_project <project_name>"
	exit -1
endif

cd ..

set GCC = /bin/gcc
set GCCOPTS = -c
set LD = /bin/gcc
set LDOPTS = -o
set PROJNAME = $1
set PROJPATH = $PWD
set MV = /bin/mv
set STAT = /bin/stat

cd ..

if (-d $PWD/$PROJNAME) then
	cd $PROJPATH/src/
	foreach file ("$PROJPATH/src"/*)
		set filename = `echo $file | cut -f 1 -d '.'`
		if ($file == $filename.c) then
			if (-e $filename.o) then
				set modtimeo = `stat --format="%Y" $filename.o`
				set modtimec = `stat --format="%Y" $filename.c`
				if($modtimec - $modtimeo > 0) then
					$GCC $GCCOPTS $filename.c
					if (! $? == 0) then
						echo "error! gcc failed to compile $file"
						exit -1
					else 
						echo "compiling $file"
					endif
				endif
			else
				$GCC $GCCOPTS $filename.c
				if (! $? == 0) then
					echo "error! gcc failed to compile $file"
					exit -1
				else 
					echo "compiling $file"
				endif
			endif
		endif
	end
	$LD $LDOPTS ../bin/$PROJNAME.out $PROJPATH/src/*.o
	if (! $? == 0) then
		echo "error! gcc failed to link the .o files"
		exit -1
	else
		echo "$PROJNAME.out has been created in the bin directory"
		exit 0
	endif
else
	echo "error! that project name does not match the folder name! build_project.csh should be in the <projname>/bin/ folder"
	exit -1
endif