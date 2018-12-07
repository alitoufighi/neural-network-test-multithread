CC=g++
C_FLAGS=-g -O0 -std=c++11
C_OBJ_FLAGS=$(C_FLAGS) -c

NeuralNetworkMultiThreadedTest: semaphore.o threads.o utils.o main.o
	$(CC) $(C_FLAGS) main.o semaphore.o threads.o utils.o -o NeuralNetworkMultiThreadedTest

main.o: main.cpp
	$(CC) $(C_OBJ_FLAGS) main.cpp

semaphore.o: semaphore.cpp semaphore.hpp
	$(CC) $(C_OBJ_FLAGS) semaphore.cpp

threads.o: threads.cpp threads.hpp utils.hpp
	$(CC) $(C_OBJ_FLAGS) threads.cpp utils.cpp
utils.o: utils.cpp utils.hpp
	$(CC) $(C_OBJ_FLAGS) utils.cpp


clean:
	rm *.o