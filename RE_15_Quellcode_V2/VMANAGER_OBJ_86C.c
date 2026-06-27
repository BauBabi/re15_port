void VMANAGER_OBJ_86C(void)

{
  int iVar1;
  undefined2 in_v0;
  int in_v1;
  
  *(undefined2 *)((int)&DAT_8008f6b2 + in_v1) = in_v0;
  (&DAT_8008f82c)[DAT_800b532e] = (&DAT_8008f82c)[DAT_800b532e] | 8;
  iVar1 = DAT_800b2b3c;
  (&DAT_8008f6b4)[DAT_800b5330] =
       *(undefined2 *)
        (((uint)DAT_800b531b * 0x10 + (uint)DAT_800b5320) * 0x20 + DAT_800b2b3c + 0x10);
  (&DAT_8008f6b6)[DAT_800b5330] =
       *(short *)(((uint)DAT_800b531b * 0x10 + (uint)DAT_800b5320) * 0x20 + iVar1 + 0x12) +
       DAT_800b2540;
  (&DAT_8008f82c)[DAT_800b532e] = (&DAT_8008f82c)[DAT_800b532e] | 0x30;
  return;
}
