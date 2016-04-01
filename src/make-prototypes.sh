#!/bin/bash
> prototypes.h
for afile in *.c;do
(
cat <<PROTO
/*
 * External prototypes for $afile
 */
#ifndef _${afile/.c/_h}_
#define _${afile/.c/_h}_
PROTO
grep -h "^RDC_EXPORT.*" $afile|sort -u|sed 's/RDC_EXPORT/extern/g;s/$/;/g'

cat <<PROTO
#endif
/*
 * Static prototypes for $afile
 */
#ifdef _${afile/.c/_c}_
PROTO

grep -h "^RDC_STATIC.*" $afile|sort -u|sed 's/RDC_STATIC/static/g;s/$/;/g'
cat <<PROTO
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
PROTO
) > ${afile/.c/_proto.h}
echo "#include \"${afile/.c/_proto.h}\"" >> prototypes.h
done
