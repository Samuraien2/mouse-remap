BIN = mouse-remap

.PHONY: clean
$(BIN): main.c
	cc main.c -o $@ -O2 -s

clean:
	rm $(BIN)
