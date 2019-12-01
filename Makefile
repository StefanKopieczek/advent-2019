SRC = src
OUTPUT = build

build/%: $(SRC)/%.c
	gcc -o $@ $<

.PHONY: clean
clean:
	rm $(OUTPUT)/*

run%:
	$(MAKE) $(OUTPUT)/$*
	./$(OUTPUT)/$*
