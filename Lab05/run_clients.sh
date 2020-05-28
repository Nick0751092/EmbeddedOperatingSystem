#!/bin/bash

for (( i=0; i<$3; i=i+1 ))
do
	./client $1 $2 ${i} &
done
