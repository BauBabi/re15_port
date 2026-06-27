echo Creating PSX libps.a ...
ren libps.a libps.old
mipsar -cq libps.a _start.o syscall.o gpu.o gte.o tmd.o int.o spu.o vablib.o font.o 
copy libps.a ..\lib
