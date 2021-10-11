CC = tcc -mh -c -v -w
LINK = tlink /c /x /v
LIB = tlib
OBJS = data.obj data2.obj scrbuff.obj cga.obj cap.obj

cap.exe: $(OBJS)
	$(LINK) $(CLIB)\c0h @cap.lnk,$*,, $(CLIB)\ch

scrbuff.obj: scrbuff.c

cap.obj: cap.c common.h cga.h data.h
data.obj: data.c common.h data.h
data2.obj: data2.c common.h data.h
cga.obj: cga.c common.h cga.h

.c.obj:
	$(CC) $<

clean:
	-del *.exe
	-del *.obj
	-del *.lib
