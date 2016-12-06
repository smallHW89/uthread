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

MY_INC= -I../../
MY_LIB= ../../libuthread.a


objects = $(C_OBJS)  $(CPP_OBJS)  $(S_OJBS)



test_app:$(C_SOURCE)  $(CPP_SOURCE) $(S_OJBS)
	$(CC)    $(MY_INC) $(MY_LIB) $^ -o $@






clean:
	rm *.o  test_app

