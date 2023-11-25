CFLAGS = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations  \
-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion 						\
-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers 			\
-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel 			\
-Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE -fsanitize=address

TEST_MAIN = ./test.exe
MAIN = ./main.exe
CPP = g++

start: tree.o test.o
	$(CPP) tree.o test.o -o $(TEST_MAIN) $(CFLAGS)
	$(TEST_MAIN)

# stack.o : stack/stack.cpp
# 	$(CPP) stack/stack.cpp -c
# 	$(CPP) stack/my_hash.cpp -c

tree.o : tree/tree.cpp
	$(CPP) tree/tree.cpp -c

# tree_dump.o : tree_dump/tree_dump.cpp
# 	$(CPP) tree_dump/tree_dump.cpp -c

test.o : test.cpp
	$(CPP) test.cpp -c

main.o : main.cpp
	$(CPP) main.cpp -c

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.out
	rm -f *.exe
	rm -f *.dot
