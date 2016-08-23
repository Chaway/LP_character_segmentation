#! /bin/sh
make;
filenames=`ls ./images/`
for input_filename in $filenames
do
	echo $input_filename
#	filename=${input_filename%.*}
#        extension=${input_filename##*.}
#	./segment $filename $extension
	./segment $input_filename
done
