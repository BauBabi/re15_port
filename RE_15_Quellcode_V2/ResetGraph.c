int ResetGraph(int mode)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = mode & 7;
  if ((uVar2 != 0) && (uVar2 != 3)) {
    if (1 < DAT_8007e352) {
      printf("ResetGraph(%d)...\n");
    }
    iVar1 = _reset(1);
    return iVar1;
  }
  printf("ResetGraph:jtb=%08x,env=%08x\n",&PTR_s_$Id:_sys.c,v_1.120_1996/05/01_12_8007e308,
         &DAT_8007e350);
  memset(&DAT_8007e350,'\0',0x80);
  ResetCallback();
  GPU_cw(0x7e308);
  DAT_8007e350 = _reset(uVar2 != 0);
  DAT_8007e351 = 1;
  DAT_8007e354 = *(undefined2 *)(&DAT_8007e3d0 + (uint)DAT_8007e350 * 4);
  DAT_8007e356 = *(undefined2 *)(&DAT_8007e3e4 + (uint)DAT_8007e350 * 4);
  memset("",0xff,0x5c);
  memset("",0xff,0x14);
  iVar1 = SYS_OBJ_140();
  return iVar1;
}
