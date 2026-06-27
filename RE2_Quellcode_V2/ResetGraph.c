/* ResetGraph @ 0x8008fee0  (Ghidra headless, raw MIPS overlay) */

int ResetGraph(int mode)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = mode & 7;
  if (uVar2 != 3) {
    if (3 < uVar2) {
      if (uVar2 != 5) {
        iVar1 = SYS_OBJ_11C();
        return iVar1;
      }
      goto SYS_OBJ_70;
    }
    if (uVar2 != 0) {
      iVar1 = SYS_OBJ_11C();
      return iVar1;
    }
  }
  printf("ResetGraph:jtb=%08x,env=%08x\n",&PTR_s__Id__sys_c_v_1_129_1996_12_25_03_800b26b8,
         &DAT_800b2700);
SYS_OBJ_70:
  memset(&DAT_800b2700,'\0',0x80);
  ResetCallback();
  GPU_cw((uint)PTR_PTR_800b26f8 & 0xffffff);
  DAT_800b2700 = _reset(mode);
  DAT_800b2701 = 1;
  DAT_800b2704 = (undefined2)*(undefined4 *)(&DAT_800b2780 + (uint)DAT_800b2700 * 4);
  DAT_800b2706 = (undefined2)*(undefined4 *)(&DAT_800b2794 + (uint)DAT_800b2700 * 4);
  memset((uchar *)&DAT_800b2710,0xff,0x5c);
  memset("",0xff,0x14);
  iVar1 = SYS_OBJ_16C();
  return iVar1;
}


