rem build example 5
rem Make sure all your paths are correct!
mipsgcc -v -O2 -Xlinker -mpsx -o example5.psx example5.c ..\lib\libps.a jum1.o
