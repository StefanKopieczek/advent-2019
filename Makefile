SRC = src
OUTPUT = build

build/%: $(SRC)/%.c
	gcc -g -std=c99 -o $@ $< $(SRC)/adventfiles.c -I $(SRC)

.PHONY: clean
clean:
	rm $(OUTPUT)/*

run%:
	$(MAKE) $(OUTPUT)/$*
	./$(OUTPUT)/$*
