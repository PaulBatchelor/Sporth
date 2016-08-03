for util in $@
do
    if [ -e $util ] 
    then
        echo install $util /usr/local/bin
        install $util /usr/local/bin
    fi
done
