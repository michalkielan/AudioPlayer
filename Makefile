TARGET=audioplayer

CC=g++
#CC=clang++

DBGFLAG=-g

STDFLAG=-std=c++14

SRC=`ls *.cpp`

CFLAGS=

LDFLAGS=

LDFLAGS+= -lasound

CFLAGS+=$(DBGFLAG)
CFLAGS+=$(STDFLAG)

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(TARGET)
