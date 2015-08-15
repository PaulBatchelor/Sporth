HEADER=sporth.h
echo "/* This is a generated header file */" > $HEADER
cat h/sporth.h >> $HEADER
csplit h/plumber.h '/* ---- */'
sh util/macro_gen.sh >> $HEADER
cat xx01 | sed "1d" >> $HEADER
rm -f xx*
