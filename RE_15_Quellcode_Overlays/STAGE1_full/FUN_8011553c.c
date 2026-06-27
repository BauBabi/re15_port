/* FUN_8011553c @ 0x8011553c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011553c(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  
  bVar1 = *(byte *)((int)_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
        *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
        if (cVar2 == '\0') {
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
          *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffff7;
        }
        *(short *)(_DAT_800ac784 + 0x23) =
             (short)_DAT_800ac784[0x23] + *(short *)((int)_DAT_800ac784 + 0x1e6);
        *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
      }
      goto LAB_801157b4;
    }
    if (bVar1 != 0) goto LAB_801157b4;
    FUN_80115d94(4);
    *(undefined2 *)(_DAT_800ac784 + 0x23) = 0x50;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1e6) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x79) = 200;
    *(undefined2 *)((int)_DAT_800ac784 + 0x1de) = 5;
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 9;
    iVar3 = func_0x8004efe4(0x800b1028,0x1e);
    if (iVar3 != 0) {
      *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
    }
  }
  *_DAT_800ac784 = *_DAT_800ac784 | 2;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
  *_DAT_800ac784 = *_DAT_800ac784 | 8;
  cVar2 = *(char *)((int)_DAT_800ac784 + 0x1d5);
  *(char *)((int)_DAT_800ac784 + 0x1d5) = cVar2 + -1;
  if (cVar2 == '\0') {
    *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0x1e;
    *(char *)((int)_DAT_800ac784 + 6) = *(char *)((int)_DAT_800ac784 + 6) + '\x01';
  }
  *(short *)(_DAT_800ac784 + 0x23) = (short)_DAT_800ac784[0x23] + 0x14;
  *(short *)(_DAT_800ac784 + 0x79) = (short)_DAT_800ac784[0x79] + -7;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x200);
LAB_801157b4:
  _DAT_800ac784[0xe] = (int)(short)_DAT_800ac784[0x79] + _DAT_800ac784[0xe];
  func_0x800245d8(0);
  return;
}


