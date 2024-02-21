# paths
LIB = ../../lib
INCLUDE = ../../include
MODULES = ../../modules
# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -Werror -g -I$(INCLUDE)
LDFLAGS = -lm


# Αρχεία .o

#για να εκτελεστει το παιχνιδι χρησημοποιώντας διαφορετική υλοποίηση αλλάζουμε τα objs σχολιάζοντας 
#την μια γραμμη και ξεσχολιάζοντας την αλλη.Τρεχουμε πρώτα make -B!!readme αρχη

OBJS = game.o $(MODULES)/state.o interface.o $(LIB)/k08.a $(LIB)/libraylib.a
#OBJS = game.o $(MODULES)/state_alt.o $(MODULES)/set_utils/set_utils.o interface.o $(LIB)/k08.a $(LIB)/libraylib.a
# Το εκτελέσιμο πρόγραμμα
EXEC = game

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS =

# Παράμετροι της βιβλιοθήκης raylib
include $(LIB)/libraylib.mk


$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	export DISPLAY=$$(ip route list default | awk '{print $$3}'):0 && \
	export LIBGL_ALWAYS_INDIRECT=1 && \
	./$(EXEC) $(ARGS)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)