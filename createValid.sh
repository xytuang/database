#!/bin/bash

names=("adam" "ruth" "hagar" "eve" "solomon" "david" "abraham" "moses")
num_rows=$1

for ((i = 1; i <= num_rows; i++)); do
    random_id=$RANDOM
    random_index=$((RANDOM % 8))
    random_name=${names[random_index]}
    echo "insert $random_id $random_name $random_name@gmail" >> input.txt
done
echo ".exit" >> input.txt
#printf "EOF" >> input.txt


