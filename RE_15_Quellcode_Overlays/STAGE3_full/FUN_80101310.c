/* FUN_80101310 @ 0x80101310  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101310(void)

{
  uint uVar1;
  uint unaff_s0;
  int iVar2;
  
  if (((_DAT_800ac784[0x76] & 0x100) != 0) && ((*_DAT_800ac784 & 0x1000) == 0)) {
    *(short *)((int)_DAT_800ac784 + 0x1da) = *(short *)((int)_DAT_800ac784 + 0x1da) + -1;
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 300) {
      iVar2 = (unaff_s0 & 0xff) * 8;
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f4)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f5)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f6)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f7)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f8)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8f9)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fa)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
      func_0x80019d50(8,3,0x16,(uint)(byte)(&DAT_8011d8fb)[iVar2] * 0xac + _DAT_800ac784[0x62] +
                               0x40);
    }
    if (*(short *)((int)_DAT_800ac784 + 0x1da) == 0) {
      _DAT_800ac784[1] = 0x1503;
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xb;
      if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x1f;
      return;
    }
  }
  uVar1 = func_0x8001bc08();
  if ((uVar1 & 0xff) >> 1 == 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] & 0xffef;
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar1 & 0xff) << 4);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) == 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0x1fffffff;
    *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  }
  func_0x80012aa4(3000);
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    *_DAT_800ac784 = *_DAT_800ac784 & 0xbfffffff;
    func_0x80012974(5000);
  }
  (*(code *)(&PTR_FUN_8011d95c)[*(byte *)((int)_DAT_800ac784 + 9) & 0xf])();
  return;
}


