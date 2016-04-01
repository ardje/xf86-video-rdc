#!/bin/sh
(
for afile in *.c;do
cat <<PROTO
/*
 * Prototypes for $afile
 */
PROTO
grep -h "^RDC_EXPORT.*" $afile|sort -u|sed 's/RDC_EXPORT/extern/g;s/$/;/g'
echo
done
echo "#define RDC_EXPORT"
) > prototypes.h
