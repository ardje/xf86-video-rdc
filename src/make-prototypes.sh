#!/bin/bash
> prototypes.h
for afile in *.c;do
(
cat <<PROTO
/*
 * Prototypes for $afile
 */
PROTO
grep -h "^RDC_EXPORT.*" $afile|sort -u|sed 's/RDC_EXPORT/extern/g;s/$/;/g'
echo
echo "#undef RDC_EXPORT"
echo "#define RDC_EXPORT"
) > ${afile/.c/_proto.h}
echo "#include \"${afile/.c/_proto.h}\"" >> prototypes.h
done
