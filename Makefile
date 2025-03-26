# Binary target
CC := g++
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb
RM := rm -f
TGTDIR := build

run: clean all
	@echo "🚀 Executing..."
	$(TGTDIR)/file-encoder

all:
	@echo "🚧 Building..."
	mkdir -p $(TGTDIR)
	$(CC) $(CXX_FLAGS) encoder/main.cpp encoder/lea9.cpp -o $(TGTDIR)/file-encoder
	@echo "Done!"

release: 
	CXX_FLAGS += -Ofast

clean:
	@echo "🧹 Clearing..."
	$(RM) $(TGTDIR)/*
