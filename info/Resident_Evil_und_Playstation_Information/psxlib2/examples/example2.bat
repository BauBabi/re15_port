rem build example 2
rem Make sure all your paths are correct!
mipsgcc -v -O2 -Xlinker -mpsx -o example2.psx example2.c ..\lib\libps.a jum303.o
