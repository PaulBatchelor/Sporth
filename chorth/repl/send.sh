str=""

while read line 
do
str="$str $line"
echo "$line"
done

echo "$str" > sporthpipe
#oscsend localhost 6449 /foo/sporth s "$str"
