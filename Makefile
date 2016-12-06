CC = g++ -g


C_SOURCE   = $(wildcard *.c)
CPP_SOURCE = $(wildcard *.cpp)
S_SOURCE       = $(wildcard *.S)

C_OBJS       = $(patsubst  %c,%o , $(C_SOURCE))
CPP_OBJS     = $(patsubst  %cpp,%o , $(CPP_SOURCE))
S_OJBS         = $(patsubst  %S,%o , $(S_SOURCE))



%.o : %.cpp 
	@echo  Compiling
	@$(CC)  -c  $< -o $@ 

%.o : %.c 
	@echo  Compiling
	@$(CC)  -c $< -o $@ 

%.o : %.S 
	@echo  Compiling
	@$(CC)  -c $< -o $@ 



objects = $(C_OBJS)  $(CPP_OBJS)  $(S_OJBS)

LIB_TARGET =  libuthread.a

all: $(LIB_TARGET)
$(LIB_TARGET):$(S_OJBS) $(C_OBJS) $(CPP_OBJS)
	ar rcs $@ $^







clean:
	rm *.o

