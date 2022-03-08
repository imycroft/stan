#!/bin/bash
for file in Malware-Feed/*/*
do  
	./stan.out -i $file -X -d M >> malset.csv
done
