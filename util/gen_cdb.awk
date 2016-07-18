# a quick parser for tinycdb
# entries look like this:
# key:value
# key2:value2
# run something like this:
# cat db.txt | awk -f gen_cdb.awk | cdb -c t.db 
BEGIN{
FS=":"
}
{
print "+" length($1) "," length($2) ":"$1"->"$2 
}
END{
print "\n"
}
