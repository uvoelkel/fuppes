libtool --mode=execute valgrind \
--tool=memcheck --leak-check=full --show-reachable=yes \
--leak-resolution=high --num-callers=40 \
--track-origins=yes \
src/fuppes &> leakcheck.txt

# --suppressions=valgrind.sup \
# --gen-suppressions=yes \
