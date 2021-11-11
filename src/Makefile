INCDIR = ../include
OBJDIR = ./obj
CFLAGS=-I$(INCDIR) -g -O0

_DEPS = cJSON.h convert.h copyTex.h extractZIP.h helper.h parse.h fixTextures.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ  = cJSON.o convert.o copyTex.o extractZIP.o helper.o parse.o fixTextures.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

LIBS = -lzip -lm

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

br-convert: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f ./br-convert $(OBJDIR)/*.o
