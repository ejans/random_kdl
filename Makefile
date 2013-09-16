ROOT_DIR=$(CURDIR)/../..
include $(ROOT_DIR)/make.conf
INCLUDE_DIR=$(ROOT_DIR)/src/
KDL_DIR=$(ROOT_DIR)/std_types/kdl/types/

TYPES:=$(wildcard types/*.h)
HEXARRS:=$(TYPES:%=%.hexarr)

random_kdl.so: random_kdl.o $(INCLUDE_DIR)/libubx.so 
	${CC} $(CFLAGS_SHARED) -o random_kdl.so random_kdl.o $(INCLUDE_DIR)/libubx.so

random_kdl.o: random_kdl.c $(INCLUDE_DIR)/ubx.h $(INCLUDE_DIR)/ubx_types.h $(INCLUDE_DIR)/ubx.c $(HEXARRS)
	${CC} -fPIC -I$(INCLUDE_DIR) -I$(KDL_DIR) -c $(CFLAGS) random_kdl.c

clean:
	rm -f *.o *.so *~ core $(HEXARRS)
