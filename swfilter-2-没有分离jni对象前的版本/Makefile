CFLAGS= -Wall -I /shiping/jdk1.7.0_79/include/ -I /usr/local/mysql/include/ -I /shiping/jdk1.7.0_79/include/linux/

all: libfilter.so test #policytest

libfilter.so: filter.c swjni.c policy.c
	gcc  $(CFLAGS)  -L /lib64 -lpthread  -ljvm -lmysqlclient -fPIC -shared filter.c swjni.c policy.c -o libfilter.so
	cp -f libfilter.so /lib64

test:
#	gcc $(CFLAGS) -o test test.c -ljvm -L ./ -lfilter -L /usr/local/mysql/lib/ -lmysqlclient -lpthread
	gcc -o test $(CFLAGS) -L /lib64 -lpthread  -ljvm -lfilter test.c

policytest:
	gcc $(CFLAGS) -o policytest policytest.c -ljvm -L ./ -lfilter -L /usr/local/mysql/lib/ -lmysqlclient -lpthread

clean:
	rm -f libfilter.so test #policytest
