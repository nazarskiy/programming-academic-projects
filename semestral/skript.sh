#!/bin/bash

output_file="excel.cpp"

tail -n +2 definitions.h > "$output_file"

tail -n +4 CPos.h >> "$output_file"

tail -n +4 CPos.cpp >> "$output_file"

tail -n +4 CSpreadsheet.h >> "$output_file"

tail -n +4 CItem.h >> "$output_file"

tail -n +4 CItem.cpp >> "$output_file"

tail -n +4 CDouble.h >> "$output_file"

tail -n +4 CDouble.cpp >> "$output_file"

tail -n +4 CString.h >> "$output_file"

tail -n +4 CString.cpp >> "$output_file"

tail -n +4 CReference.h >> "$output_file"

tail -n +4 CReference.cpp >> "$output_file"

tail -n +4 COperator.h >> "$output_file"

tail -n +4 COperator.cpp >> "$output_file"

tail -n +4 CAdd.h >> "$output_file"

tail -n +4 CAdd.cpp >> "$output_file"

tail -n +4 CSub.h >> "$output_file"

tail -n +4 CSub.cpp >> "$output_file"

tail -n +4 CMul.h >> "$output_file"

tail -n +4 CMul.cpp >> "$output_file"

tail -n +4 CDiv.h >> "$output_file"

tail -n +4 CDiv.cpp >> "$output_file"

tail -n +4 CPow.h >> "$output_file"

tail -n +4 CPow.cpp >> "$output_file"

tail -n +4 CNeg.h >> "$output_file"

tail -n +4 CNeg.cpp >> "$output_file"

tail -n +4 CEq.h >> "$output_file"

tail -n +4 CEq.cpp >> "$output_file"

tail -n +4 CNe.h >> "$output_file"

tail -n +4 CNe.cpp >> "$output_file"

tail -n +4 CLt.h >> "$output_file"

tail -n +4 CLt.cpp >> "$output_file"

tail -n +4 CLe.h >> "$output_file"

tail -n +4 CLe.cpp >> "$output_file"

tail -n +4 CGt.h >> "$output_file"

tail -n +4 CGt.cpp >> "$output_file"

tail -n +4 CGe.h >> "$output_file"

tail -n +4 CGe.cpp >> "$output_file"

tail -n +18 CMyBuilder.h >> "$output_file"

tail -n +4 CMyBuilder.cpp >> "$output_file"

tail -n +5 CSpreadsheet.cpp >> "$output_file"

sed -n '42,261p' test.cpp  >> "$output_file"

g++ -std=c++20 -Wall -pedantic -g -o excel -fsanitize=address excel.cpp -L./x86_64-linux-gnu -lexpression_parser


















