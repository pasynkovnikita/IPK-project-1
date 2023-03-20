ifeq ($(OS),Windows_NT)
run:
	gcc client.c -o ipkcpc -lws2_32
else
run:
	gcc client.c -o ipkcpc
endif