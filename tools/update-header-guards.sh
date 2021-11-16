#!/bin/sh

# A (very) crude script to update header guards all across project

for header in $(find -name "*.h"); do
	header_guard=$(grep "#ifndef .*_H$" $header | cut -sd ' ' -f2)
	[ -z "$header_guard" ] && continue

	correct_header_guard=$(echo $header | sed "s/\.\///g;s/[\/.]/_/g" | tr 'a-z' 'A-Z')
	sed -i "s/${header_guard}\$/$correct_header_guard/g" $header
done
