
if [ "$PYTHONPATH" == "" ]; then
    echo "empty"
    export PYTHONPATH=/usr/local/lib
else
    echo "has content"
    echo $PYTHONPATH
fi
#PYTHONPATH=$PYTHONPATH:/usr/local/lib
