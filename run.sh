S=$(printf "%04d\n" "${1}")

./AHC020 < in/${S}.txt > out/${S}.txt 2>> res/${S}.txt