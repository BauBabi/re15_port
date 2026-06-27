/* FUN_80081cec @ 0x80081cec  (Ghidra headless, raw MIPS overlay) */

void FUN_80081cec(void)

{
  undefined2 uVar1;
  uint *puVar2;
  int iVar3;
  
  iVar3 = 0;
  puVar2 = (uint *)&DAT_800eae58;
  DAT_800dcc4c = DAT_800dcc4a * 8;
  DAT_800dcc4e = (ushort)DAT_800dcc3c + (ushort)DAT_800dcc37 * 0x10;
  (&DAT_800cbcd6)[DAT_800dcc4a * 0x1b] = 0x7fff;
  do {
    iVar3 = iVar3 + 1;
    *puVar2 = *puVar2 & ~(1 << ((int)DAT_800dcc4a & 0x1fU));
    puVar2 = puVar2 + 1;
  } while (iVar3 < 0x10);
  if ((DAT_800dcc48 & 1) == 0) {
    uVar1 = *(undefined2 *)((((short)DAT_800dcc48 + -1) / 2) * 0x10 + DAT_800d7838 + 0xe);
  }
  else {
    uVar1 = *(undefined2 *)((((short)DAT_800dcc48 + -1) / 2) * 0x10 + DAT_800d7838 + 0xc);
  }
  (&DAT_800dccb6)[DAT_800dcc4c] = uVar1;
  (&DAT_800d4c80)[DAT_800dcc4a] = (&DAT_800d4c80)[DAT_800dcc4a] | 8;
  iVar3 = DAT_800d784c;
  (&DAT_800dccb8)[DAT_800dcc4c] =
       *(undefined2 *)
        (((uint)DAT_800dcc37 * 0x10 + (uint)DAT_800dcc3c) * 0x20 + DAT_800d784c + 0x10);
  (&DAT_800dccba)[DAT_800dcc4c] =
       *(short *)(((uint)DAT_800dcc37 * 0x10 + (uint)DAT_800dcc3c) * 0x20 + iVar3 + 0x12) +
       DAT_800d52b0;
  (&DAT_800d4c80)[DAT_800dcc4a] = (&DAT_800d4c80)[DAT_800dcc4a] | 0x30;
  return;
}


