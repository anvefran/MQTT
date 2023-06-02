DEPS = common.h
CFLAGS = -Wall -c

.PHONY: all debug sanitize clean
all: broker subscriber publisher

broker: broker.o common.o $(DEPS)
	gcc -o $@ broker.o common.o $(DFLAGS) -lpthread

subscriber: subscriber.o common.o $(DEPS)
	gcc -o $@ subscriber.o common.o $(DFLAGS)

publisher: publisher.o common.o
	gcc -o $@ publisher.o common.o $(DFLAGS)

%.o: %.c $(DEPS)
	gcc $(CFLAGS) $< $(DFLAGS)

# Compila usando la opción -g para facilitar la depuración con gdb.
debug: DFLAGS = -g
debug: clean all

# Compila habilitando la herramienta AddressSanitizer para
# facilitar la depuración en tiempo de ejecución.
sanitize: DFLAGS = -fsanitize=address,undefined
sanitize: clean all

clean:
	rm -rf broker subscriber publisher *.o
