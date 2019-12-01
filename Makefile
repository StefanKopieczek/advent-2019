SRC = src
OUTPUT = build

build/%: $(SRC)/%.c
	gcc -o $@ $< $(SRC)/adventfiles.c -I $(SRC)

.PHONY: clean
clean:
	rm $(OUTPUT)/*

run%:
	$(MAKE) $(OUTPUT)/$*
	./$(OUTPUT)/$*
