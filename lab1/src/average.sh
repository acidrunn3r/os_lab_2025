#!/bin/bash

amount=$#
avg=0
for i in {0..$amount}
do
    echo $i
    avg=$avg+$i
done
avg=$avg/$#
echo "Total Number of Parameters: $#"
echo "Total avg: $avg"