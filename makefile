#Grupo: 35
#David Dantas 56331
#Su Lishun 56375
#Miguel Seabra 56344

OBJECTOS = main.o configuration.o memory.o process.o client.o driver.o restaurant.o synchronization.o metime.o mesignal.o log.o stats.o 
CC = gcc
main.o = main.h process.h
memory.o = memory.h
process.o = process.h restaurant.h driver.h client.h
restaurant.o = restaurant.h
driver.o = driver.h
client.o = client.h 
synchronization.o = synchronization.h
metime.o = metime.h
mesignal.o = mesignal.h log.h
log.o = log.h
stats.o = stats.h main.h
configuration.o = configuration.h
OBJ_dir = obj
SRC_dir = src
LIBS = -lrt -lpthread
BIN_dir = bin
magnaeats: $(OBJECTOS)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJECTOS)) -g  -Wall -o $(BIN_dir)/magnaeats $(LIBS)
%.o: $(SRC_dir)/%.c $($@)
	$(CC) -I include -o $(OBJ_dir)/$@ -c $<	
clean: 
	rm -f ./$(OBJ_dir)/*.o ./$(BIN_dir)/magnaeats
	rm -f ./$(BIN_dir)/magnaeats
