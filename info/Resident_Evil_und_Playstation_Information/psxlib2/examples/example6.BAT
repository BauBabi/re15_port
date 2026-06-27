rem build example 6
rem Make sure all your paths are correct!
mipsgcc -v -O2 -Xlinker -mpsx -o example6.psx example6.c ..\lib\libps.a sqr440.o sine440.o