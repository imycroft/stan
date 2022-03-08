#!/bin/bash
for file in Malware-Feed/*/*
do  
	./stan.out -i $file -P

	if [ $? -ne 1 ]
	then
		rm $file
	fi
done
