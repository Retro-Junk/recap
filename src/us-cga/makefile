CC = tcc -mh -c -v -w
LINK = tlink /c /x /v
LIB = tlib
OBJS = data.obj data2.obj scrbuff.obj cga.obj cap.obj script.obj timer.obj input.obj hand.obj sound.obj

cap.exe: $(OBJS)
	$(LINK) $(CLIB)\c0h @cap.lnk,$*,, $(CLIB)\ch

scrbuff.obj: scrbuff.c

cap.obj: cap.c common.h cga.h data.h
data.obj: data.c common.h data.h
data2.obj: data2.c common.h data.h
cga.obj: cga.c common.h cga.h
script.obj: script.c common.h script.h
timer.obj: timer.c common.h timer.h
input.obj: input.c common.h input.h
hand.obj: hand.c common.h hand.h
sound.obj: input.c common.h sound.h

.c.obj:
	$(CC) $<

clean:
	-del *.exe
	-del *.obj
	-del *.lib
