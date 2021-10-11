@echo off
copy /y CAP.EXE ..\..\..\dbx\disk\cabl
pushd ..\..\..\dbx
dosbox -c "cd cabl" -c cap.exe
popd
