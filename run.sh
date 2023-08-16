S=$(printf "%04d\n" "${1}")

./AHC020 < in/${S}.txt > out/${S}.out 2>> res/${S}.res