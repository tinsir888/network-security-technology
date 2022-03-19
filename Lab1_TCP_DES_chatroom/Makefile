chat: main.o server.o client.o des_algorithm.o type_conversion.o des_encryption.o des_decryption.o generate_subkey.o des_f_function.o
	g++ -o chat main.o server.o client.o des_algorithm.o type_conversion.o des_encryption.o des_decryption.o generate_subkey.o des_f_function.o
.PHONY:clean, run
clean:
	-rm -rf *.o chat
run:chat
	./chat
