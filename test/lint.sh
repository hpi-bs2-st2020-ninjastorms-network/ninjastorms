#!/bin/bash

indent -version

# for now only include our source files to avoid huge merge conflicts
include_files=(\
	"./kernel/logger/*.c" "./kernel/logger/*.h" \
	"./kernel/network/*.c" "./kernel/network/*.h" \
	"./kernel/pci/*.c" "./kernel/pci/*.h" \
	"./kernel/time.c" "./kernel/pci/time.h" \
	"./libc/unistd/*.c" "./libc/include/unistd.h" \
)

status=0
files=$(printf ' -o -path "%s"' "${include_files[@]}")
while read file; do
    indent $file -o "$file.indented.c"
    output=$(diff -u --color='auto' $file "$file.indented.c")
    # if files differ set status and print output
    if [[ -n "$output" ]]; then
    	status=1
    	echo "$output"
    fi
    rm "$file.indented.c"
done < <(eval "find . -type f \( -path "" "$files" \) -printf '%h\0%d\0%p\n'" | sort -t '\0' -n | awk -F '\0' '{print $3}')

exit $status
