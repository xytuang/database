#!/bin/bash

input_file="input.txt"
output_file="output.txt"

rm -f "$output_file" #removes any existing output file so brand new output file is created

commands=("createValid.sh" "createMalformed.sh")

run_command=${commands[1]} #change the index into command to choose what test case you want to run

read num_rows

./"$run_command" "$num_rows"


fifo="sqlite_fifo"

# Create a named pipe
mkfifo "$fifo"

# Start ./sqlite in the background, reading from the named pipe
./sqlite < "$fifo" >> "$output_file" &

# Read each line from input_file and write it to the named pipe
while IFS= read -r line; do
    echo "$line" > "$fifo"
done < "$input_file"

# Remove intermediate files
rm "$fifo"

rm "$input_file"
