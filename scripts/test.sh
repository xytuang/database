#!/bin/bash

input_file="input.txt"
output_file="output.txt"

exec 3<"$input_file"
exec 4<"$output_file"


#valid_test() {}

malformed_test() {
    malformed_output=${1:8}
    if [ "$malformed_output" == "Prepare Failure" ]; then
        echo "PASS"
    else
        echo "FAIL"
    fi
}


for i in range{1..5}; do
    read -u 4 output_line
done

while true; do
    
    read -u 3 input_line
    read -u 4 output_line

    if [ -z "$input_line" ] && [ -z "$output_line" ]; then
        break
    fi

    #if [ "$1" == "0" ]; then #0 indicates that valid input was created
    #    valid_test()
    if [ "$1" == "1" ]; then #1 indicates that malformed input was created
        malformed_test "$output_line"
    fi

done
exec 3<&-
exec 4<&-

rm "$output_file"
echo "Finished test"
exit 0
