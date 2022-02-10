
#! make -f

DIRS = bfgw changedbpwd daemon monisvr monitor sgemonitor GTP kvdata
all:
	for dir in $(DIRS); do\
		echo Making all in $$dir... ;\
		(cd $$dir && make clean && make );\
		if test $$? != 0 ; then exit 1; fi ;\
	done
clean:
	for dir in $(DIRS); do\
		echo clean all in $$dir... ;\
		(cd $$dir && make clean);\
	done


