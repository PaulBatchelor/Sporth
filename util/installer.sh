for util in $@
do
    if [ -e $util ] 
    then
        install $util /usr/local/bin
    fi
done
