BIN = mouse_remap

.PHONY: all setup clean
all:
	gcc main.c -o $(BIN)

setup:
	sudo cp $(BIN) /usr/local/bin/$(BIN)
	sudo chmod +x /usr/local/bin/$(BIN)

clean:
	rm $(BIN)