void DumpClut(u_short clut)

{
  printf("clut: (%d,%d)\n",(clut & 0x3f) << 4,(uint)(clut >> 6));
  return;
}
