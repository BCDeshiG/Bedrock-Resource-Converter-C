INCDIR = ../include
OBJDIR = ./obj
CFLAGS=-I$(INCDIR) -g -O0

_DEPS = cJSON.h convert.h copyTex.h extractArchive.h fixTextures.h fixChests.h \
		helper.h parse.h split.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ  = cJSON.o convert.o copyTex.o extractArchive.o fixTextures.o fixChests.o \
		helper.o parse.o split.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

LIBS = -larchive -lm

$(OBJDIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

br-convert: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f ./br-convert $(OBJDIR)/*.o
