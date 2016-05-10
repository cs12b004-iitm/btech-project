#!/bin/sh


for FILE in ./Data/*
do
	./CacheMultiRegular -a 8 -F $FILE -N 4 >> Results/4_8_multiReg.txt
	./CacheMultiReuse -a 8 -F $FILE -N 4 >> Results/4_8_multiReu.txt
	./CacheSingleRegular -a 8 -F $FILE >> Results/4_8_singleReg.txt
	./CacheSingleRegular -a 8 -F $FILE >> Results/4_8_singleReu.txt	
done