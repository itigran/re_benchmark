SOURCES=re_benchmark.cc
LIBS=-lpcre -lre2
TARGET=re_benchmark

$(TARGET): $(SOURCES)
	g++ -ggdb -o $(TARGET) $(SOURCES) $(LIBS)

callgraph: $(TARGET)
	valgrind --tool=callgrind ./$(TARGET) ./re_benchmark --regex-file tests/simple.txt --text-file re_benchmark.cc  --test-count 100
	
memleaks: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) ./re_benchmark --regex-file tests/simple.txt --text-file re_benchmark.cc  --test-count 100
