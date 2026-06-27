/* FUN_80113590 @ 0x80113590  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80113590(void)

{
  byte bVar1;
  ushort uVar2;
  int iVar3;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        return;
      }
      func_0x8001a8f8(&DAT_800aca88,0x32);
      func_0x800245d8(0);
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      iVar3 = func_0x8001a804(9000,100,&DAT_800aca88);
      if (-1 < iVar3) {
        return;
      }
      FUN_80115d74(0xb);
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    FUN_80115d94(6);
    func_0x800453d0(1);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
  }
  func_0x8001a8f8(&DAT_800aca88,0x32);
  if (*(short *)(_DAT_800ac784 + 0x1ea) + -0xe10 < *(int *)(_DAT_800ac784 + 0x38)) {
    *(ushort *)(_DAT_800ac784 + 0x1e4) = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
    uVar2 = -*(short *)(_DAT_800ac784 + 0x1e4);
  }
  else {
    if (*(short *)(_DAT_800ac784 + 0x1ea) + -0x1518 <= *(int *)(_DAT_800ac784 + 0x38)) {
      *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x32;
      *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + '\x01';
      goto LAB_801136dc;
    }
    uVar2 = *(byte *)(_DAT_800ac784 + 0x1d4) & 0x3f;
  }
  *(ushort *)(_DAT_800ac784 + 0x1e4) = uVar2;
  *(int *)(_DAT_800ac784 + 0x38) =
       (int)*(short *)(_DAT_800ac784 + 0x1e4) + *(int *)(_DAT_800ac784 + 0x38);
LAB_801136dc:
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  return;
}


