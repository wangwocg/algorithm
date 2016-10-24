all: test
test: libswfilter.so
	gcc -o test -I /shiping/jdk1.7.0_79/include/  -I /shiping/jdk1.7.0_79/include/linux/  -L /lib64 -lpthread  -ljvm -lswfilter test.c
libswfilter.so: keywordfilter.c swjni.c filter.c
	gcc  -I /shiping/jdk1.7.0_79/include/  -I /shiping/jdk1.7.0_79/include/linux/  -L /lib64 -lpthread  -ljvm -lmysqlclient -fPIC -shared keywordfilter.c swjni.c filter.c -o libswfilter.so
	rm -f /lib64/libswfilter.so
	cp libswfilter.so  /lib64/
clean:
	rm -f libswfilter.so test
