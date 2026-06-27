rem build example 7
rem Make sure all your paths are correct!
mipsgcc -v -O2 -I..\include -Xlinker -mpsx -o example7.psx example7.c ..\lib\libps.a term1220.o mpatrol.o star.o