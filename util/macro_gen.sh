echo "enum {"
echo "SP_DUMMY = SPORTH_FOFFSET - 1,"
cat h/ugens.h |\
sed 's/SPORTH_UGEN(\(.*\))/\1/' |\
sed "s/ //g" | cut -d ',' -f 3 |\
sed "s/$/,/"
echo "SPORTH_LAST"
echo "};"
