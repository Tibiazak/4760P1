all: master producer consumer

master: main.o
	gcc -Wall -lrt -o master main.o

main.o: main.c
	gcc -Wall -c -lrt main.c

producer: producer.o
	gcc -Wall -o producer producer.o

producer.o: producer.c
	gcc -Wall -c producer.c

consumer: consumer.o
	gcc -Wall -o comsumer consumer.o

consumer.o: consumer.c
	gcc -Wall -c consumer.c

clean:
	rm *.o master producer consumer

