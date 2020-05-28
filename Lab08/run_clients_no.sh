#!/bin/bash

for (( i=0; i<$3; i=i+1 ))
do
	./client $1 $2 ${i} &
done

for (( i=0; i<$4; i=i+1 ))
do
	./client_no $1 $2 ${i} &
done
