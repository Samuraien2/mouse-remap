BIN = mouse-remap

.PHONY: clean
$(BIN):
	cc main.c -o $@ -O2 -s

clean:
	rm $(BIN)
