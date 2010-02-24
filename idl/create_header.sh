#!/bin/bash

file=vtkWrapIDL.h
wfile=$1/wrapfiles.txt

echo 'const char* wrapped_classes[] = {' > $file
awk '{print "\""$1"\",";}' $wfile >> $file
echo '""' >> $file
echo '};' >> $file
