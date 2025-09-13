BIN = mouse-remap

.PHONY: clean
$(BIN):
	gcc main.c -o $@

clean:
	rm $(BIN)
