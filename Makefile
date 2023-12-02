DEBUG = -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations  \
-Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wconversion 						\
-Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers 			\
-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel 			\
-Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE -fsanitize=address

TEST_MAIN = ./test.exe
MAIN = ./main.exe
CPP = g++

start: tree.o tree_dump.o differentiator.o tools.o main.o
	$(CPP) tree.o tree_dump.o differentiator.o tools.o test.o -o $(TEST_MAIN) $(DEBUG)
	$(TEST_MAIN)

tree.o : tree/tree.cpp
	$(CPP) tree/tree.cpp -c

tree_dump.o : tree/tree_dump/tree_dump.cpp
	$(CPP) tree/tree_dump/tree_dump.cpp -c

differentiator.o : differentiator/differentiator.cpp
	$(CPP) differentiator/differentiator.cpp -c

tools.o : tools/tools.cpp
	$(CPP) tools/tools.cpp -c

main.o : test.cpp
	$(CPP) test.cpp -c

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f tree/tree_dump/dumps/png/*.*
	rm -f tree/tree_dump/dumps/dot_files/*.*
	rm -f tree/tree_dump/dumps/dumps/*.*
	rm -f tree/tree_dump/dumps/tex/*.*
	rm -f *.out
	rm -f *.exe
	rm -f *.dot
	rm -f tree/tree_dump/dumps/pdf/*.aux
	rm -f tree/tree_dump/dumps/pdf/*.log
	rm -f tree/tree_dump/dumps/pdf/*.pdf

