#!/bin/bash

sum=0
count=$#

for param in "$@"
do
    #echo "$param"
    sum=$((sum + param))
done

avg=$((sum / count))
#echo "cnt: $count"
echo "sum: $sum"
echo "avg: $avg"