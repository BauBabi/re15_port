/* FUN_8001b934 @ 0x8001b934  (Ghidra headless, raw MIPS overlay) */

void FUN_8001b934(int param_1,undefined4 param_2)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  
  if (DAT_800cfc00 != 0xd) {
    if (0xd < DAT_800cfc00) {
      if (DAT_800cfc00 != 0xe) goto LAB_8001b978;
LAB_8001ba14:
      DAT_800d531c = 0x1d6;
      DAT_800d5308 = *(undefined4 *)(&DAT_8009b884 + param_1 * 8);
      uVar2 = 0x1d6;
      iVar3 = *(int *)(&DAT_8009b880 + param_1 * 8);
      DAT_8009975b = (&DAT_8009c1e0)[param_1];
      iVar1 = (uint)DAT_8009975a * 0x10000 + (uint)DAT_80099758;
      DAT_800d531e = (&DAT_8009c1e0)[param_1];
      goto LAB_8001ba90;
    }
    if (DAT_800cfc00 != 0xb) {
LAB_8001b978:
      if ((DAT_800cfc00 & 1) != 0) goto LAB_8001ba14;
    }
  }
  DAT_800d531c = 0x1d5;
  DAT_800d5308 = *(undefined4 *)(&DAT_8009adf8 + param_1 * 8);
  uVar2 = 0x1d5;
  iVar3 = *(int *)(&DAT_8009adf4 + param_1 * 8);
  DAT_80099753 = (&DAT_8009b754)[param_1];
  iVar1 = (uint)DAT_80099752 * 0x10000 + (uint)DAT_80099750;
  DAT_800d531e = (&DAT_8009b754)[param_1];
LAB_8001ba90:
  DAT_800d5314 = iVar3 + iVar1;
  FUN_80012fb8(uVar2,param_2,3,&DAT_800108c8);
  return;
}


