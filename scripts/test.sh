#/bin/bash

input_file="input.txt"
output_file="output.txt"

exec 3<"$input_file"
exec 4<"$output_file"

num_case=0

valid_test() {
    if [ "$2" != "sqlite> Insert Success" ]; then
        echo "Not success"
        return
    fi

    read -a input_fields <<< "$1"

    read -a output_id <<< "$3"
    read -a output_username <<< "$4"
    read -a output_email <<< "$5"

    if [ ${output_id[1]} != ${input_fields[1]} ]; then
        echo "FAIL: Wrong id"
        return
    fi

    if [ ${output_username[1]} != ${input_fields[2]} ]; then
        echo "FAIL: Wrong username"
        return
    fi

    if [ ${output_email[1]} != ${input_fields[3]} ]; then
        echo "FAIL: Wrong email"
        return
    fi

    if [ "$6" != "Executed" ]; then
        echo "FAIL: Not executed"
        return
    fi
    echo "PASS"
}

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

    if [ "$input_line" == ".exit" ] && [ "$output_line"  == "sqlite> Exiting" ]; then
        break
    fi

    ((num_case++))
    echo "**** TEST CASE $num_case ****"
    echo "INPUT_LINE: $input_line"
    if [ "$1" == "0" ]; then #0 indicates that valid input was created

        read -u 4 output_id
        read -u 4 output_username
        read -u 4 output_email
        read -u 4 execute_line

        echo "OUTPUT_LINE:"
        echo "$output_line"
        echo "$output_id"
        echo "$output_username"
        echo "$output_email"

        valid_test "$input_line" "$output_line" "$output_id" "$output_username" "$output_email" "$execute_line"
    elif [ "$1" == "1" ]; then #1 indicates that malformed input was created
        echo "OUTPUT_LINE: $output_line"
        malformed_test "$output_line" 
    fi
    echo "*****************************"

done
exec 3<&-
exec 4<&-

#rm "$output_file"
echo "Finished test"
exit 0
