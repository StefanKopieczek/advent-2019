SRC = src
OUTPUT = build

build:
	mkdir build

build/%: $(SRC)/%.c build
	gcc -Wall -lm -g -std=c99 -o $@ $< $(SRC)/adventfiles.c -I $(SRC)

.PHONY: clean
clean:
	rm -rf $(OUTPUT)/*

run%:
	$(MAKE) $(OUTPUT)/$*
	./$(OUTPUT)/$*
