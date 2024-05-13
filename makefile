build: parent produser consumer

parent: main.cc parent.cc
	gcc -o parent $^

produser: produser.cc
	gcc -o produser $^

consumer: consumer.cc
	gcc -o consumer $^
	