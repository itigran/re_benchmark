SOURCES=re_benchmark.cc
LIBS=-lpcre -lre2
TARGET=re_benchmark

$(TARGET): $(SOURCES)
	g++ -ggdb -o $(TARGET) $(SOURCES) $(LIBS)
