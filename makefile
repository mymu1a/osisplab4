build: parent produser consumer

parent: main.cc parent.cc circleQueue.cc
	gcc -o parent $^

produser: produser.cc circleQueue.cc child.cc
	gcc -o produser $^

consumer: consumer.cc circleQueue.cc child.cc
	gcc -o consumer $^
	