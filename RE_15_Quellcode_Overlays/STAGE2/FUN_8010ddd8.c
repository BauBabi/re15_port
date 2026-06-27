/* FUN_8010ddd8 @ 0x8010ddd8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined8 FUN_8010ddd8(void)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  
  bVar5 = *(byte *)(_DAT_800ac784 + 7);
  uVar4 = (uint)(bVar5 < 2);
  if (bVar5 != 1) {
    if (uVar4 == 0) {
      uVar4 = 2;
      if (bVar5 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
        if (*(char *)(_DAT_800ac784 + 0x1e0) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 4) = 1;
          iVar2 = _DAT_800ac784;
          *(undefined1 *)(_DAT_800ac784 + 5) = 4;
          *(undefined1 *)(_DAT_800ac784 + 6) = 0;
          iVar3 = _DAT_800ac784;
          *(undefined1 *)(_DAT_800ac784 + 7) = 0;
          return CONCAT44(iVar2,iVar3);
        }
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        return CONCAT44(_DAT_800ac784,2);
      }
      goto LAB_8010e738;
    }
    if (bVar5 != 0) goto LAB_8010e738;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0xa54,0x80118bb8);
  }
  uVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  bVar5 = *(char *)(_DAT_800ac784 + 7) + (char)uVar4;
  *(byte *)(_DAT_800ac784 + 7) = bVar5;
LAB_8010e738:
  *(undefined1 *)(uVar4 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 0;
  *(byte *)(_DAT_800ac784 + 0x93) = bVar5;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x78;
  FUN_8011089c(0);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  bVar5 = 0;
  uVar4 = 0;
  do {
    bVar5 = bVar5 + 1;
    *(undefined4 *)(uVar4 * 4 + _DAT_800ac784 + 0x1d0) = 0;
    uVar4 = (uint)bVar5;
  } while (bVar5 < 8);
  _DAT_80119318 = *(uint **)(_DAT_800ac784 + 0x188);
  *_DAT_80119318 = *_DAT_80119318 & 0xfffffffe;
  *(undefined4 *)(_DAT_800ac784 + 4) = 1;
  iVar2 = _DAT_800ac784;
  pbVar1 = (byte *)(_DAT_800ac784 + 9);
  *(byte *)(_DAT_800ac784 + 5) = *pbVar1;
  return CONCAT44(iVar2,(uint)*pbVar1);
}


