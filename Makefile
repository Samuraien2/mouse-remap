BIN = mouse-remap

.PHONY: clean
$(BIN):
	cc main.c -o $@

clean:
	rm $(BIN)
