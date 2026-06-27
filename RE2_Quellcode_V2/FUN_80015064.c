/* FUN_80015064 @ 0x80015064  (Ghidra headless, raw MIPS overlay) */

void FUN_80015064(void)

{
  int iVar1;
  int iVar2;
  
  iVar2 = (uint)*(byte *)(DAT_800ce550 + 0xc) * 0xc;
  iVar1 = (uint)*(ushort *)(&DAT_8009a4b0 + (uint)*(byte *)(DAT_800ce550 + 0xc) * 2) * 8;
  DAT_800d531e = (&DAT_8009a529)[iVar2];
  DAT_800d5314 = (uint)*(ushort *)(&DAT_800988a8 + iVar1) +
                 (uint)(byte)(&DAT_800988aa)[iVar1] * 0x10000 + *(int *)(&DAT_8009a524 + iVar2);
  DAT_800d5308 = (uint)*(ushort *)(&DAT_8009a522 + iVar2);
  DAT_800cfbd8 = DAT_800cfbd8 | 0x20000;
  FUN_80012fb8((uint)*(ushort *)(&DAT_8009a4b0 + (uint)*(byte *)(DAT_800ce550 + 0xc) * 2),
               &DAT_801a1000,3,"DOOR TEXTURE");
  return;
}


