#CFLAGS=-Wall -Werror -O3
CFLAGS=-Wall -O3
LDFLAGS=-lm -lnetcdf

all: vic_classic_to_image

vic_classic_to_image: \
	main.o \
	double_stack.o \
	global_params.o \
	soil.o \
	veg_lib.o \
	veg_params.o \
	image_domain.o \
	image_params.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	$(RM) *.o
