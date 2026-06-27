/* FUN_8008156c @ 0x8008156c  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_8008156c(ushort param_1,undefined2 param_2,undefined2 param_3)

{
  byte bVar1;
  undefined4 uVar2;
  int iVar3;
  
  if (param_1 < 0x18) {
    iVar3 = (int)(short)param_1;
    (&DAT_800dccb2)[iVar3 * 8] = param_3;
    bVar1 = (&DAT_800d4c80)[iVar3];
    uVar2 = 0;
    (&DAT_800dccb0)[iVar3 * 8] = param_2;
    (&DAT_800d4c80)[iVar3] = bVar1 | 3;
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}


