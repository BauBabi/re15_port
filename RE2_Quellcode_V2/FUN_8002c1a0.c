/* FUN_8002c1a0 @ 0x8002c1a0  (Ghidra headless, raw MIPS overlay) */

void FUN_8002c1a0(uint param_1,ushort param_2,uint param_3,undefined1 param_4)

{
  int iVar1;
  uint uVar2;
  
  uVar2 = param_1 & 0xff;
  iVar1 = uVar2 * 0x4c;
  (&DAT_800dfc1e)[uVar2 * 0x26] = param_2;
  (&DAT_800dfc20)[iVar1] = (char)(param_1 >> 8);
  (&DAT_800dfc24)[iVar1] = param_4;
  if ((param_3 & 4) == 0) {
    (&DAT_800dfc21)[iVar1] = 0;
  }
  else {
    (&DAT_800dfc21)[iVar1] = 0xff;
  }
  if ((param_3 & 2) == 0) {
    (&DAT_800dfc22)[iVar1] = 0;
  }
  else {
    (&DAT_800dfc22)[iVar1] = 0xff;
  }
  if ((param_3 & 1) == 0) {
    (&DAT_800dfc23)[iVar1] = 0;
  }
  else {
    (&DAT_800dfc23)[iVar1] = 0xff;
  }
  SetDrawMode((DR_MODE *)(&UNK_800dfc48 + iVar1),1,0,(uint)(byte)(&DAT_800dfc20)[iVar1] << 5,
              (RECT *)0x0);
  SetDrawMode((DR_MODE *)(&UNK_800dfc54 + iVar1),1,0,(uint)(byte)(&DAT_800dfc20)[iVar1] << 5,
              (RECT *)0x0);
  (&DAT_800dfc64)[uVar2 * 0x13] = 0xf00140;
  (&DAT_800dfc60)[uVar2 * 0x13] = 0;
  if ((int)((uint)param_2 << 0x10) < 0) {
    (&DAT_800dfc1c)[uVar2 * 0x26] = param_2 + 0x8000;
  }
  else {
    (&DAT_800dfc1c)[uVar2 * 0x26] = 0;
  }
  return;
}


