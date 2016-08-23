 #! /bin/sh
 make;
 dir="./images/"
 echo $dir
 filenames=`ls $dir`
 for input_filename in $filenames
 do
         echo ${dir}${input_filename}
 #       filename=${input_filename%.*}
 #        extension=${input_filename##*.}
 #       ./segment $filename $extension
        ./segment ${dir}${input_filename}                                             
 done