#!/bin/sh
(
grep -h "^RDC_EXPORT.*" *.c|sort -u|sed 's/RDC_EXPORT/extern/g;s/$/;/g'
echo "#define RDC_EXPORT"
) > prototypes.h
