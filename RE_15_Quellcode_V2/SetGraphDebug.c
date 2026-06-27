int SetGraphDebug(int level)

{
  uint uVar1;
  
  uVar1 = (uint)DAT_8007e352;
  DAT_8007e352 = (byte)level;
  if ((level & 0xffU) != 0) {
    printf("SetGraphDebug:level:%d,type:%d reverse:%d\n",level & 0xff,(uint)DAT_8007e350,
           (uint)DAT_8007e353);
  }
  return uVar1;
}
