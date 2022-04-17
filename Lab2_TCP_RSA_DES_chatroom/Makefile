chat: *.cpp des/*.cpp rsa/*.cpp
	g++ *.cpp des/*.cpp rsa/*.cpp -std=c++11 -o chat
.PHONY: clean, run
clean:
	-rm -rf *.o chat
run:
	g++ *.cpp des/*.cpp rsa/*.cpp -std=c++11 -o chat
	./chat