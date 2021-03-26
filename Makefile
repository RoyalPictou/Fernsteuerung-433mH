APP := run
	
SRCS := control.cpp i2c_lcd1602.cpp
OBJS := $(SRCS:.cpp=.o) rc-switch/RCSwitch.o

CFLAGS :=-DRPI
LDFLAGS := -lwiringPi -lwiringPiDev -lcrypt 


all: $(APP)

%.o: %.cpp	
	gcc $(CFLAGS) -c $<

$(APP): $(OBJS)
	gcc -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf $(APP) $(OBJS)
