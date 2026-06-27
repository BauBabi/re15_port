/* FUN_801190dc @ 0x801190dc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801190dc(void)

{
  byte bVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  iVar3 = *(int *)(in_v0 + 0x188);
  iVar2 = (uint)*(ushort *)(iVar3 + 0xa92) - (uint)*(ushort *)(in_v0 + 0x9c);
  *(short *)(iVar3 + 0xa92) = (short)iVar2;
  if (iVar2 * 0x10000 < 0) {
    *(undefined2 *)(iVar3 + 0xa92) = 0;
  }
  func_0x8003a95c(iVar3 + 0xa14,1);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
  if (*(short *)(iVar3 + 0xa92) < 1) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 6;
  }
  iVar2 = *(int *)(_DAT_800ac784 + 0x84);
  func_0x8001f314(iVar2,*(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x100);
  bVar1 = *(byte *)(iVar2 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      return;
    }
    if (bVar1 != 0) {
      return;
    }
    *(undefined1 *)(iVar2 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(byte *)(_DAT_800ac784 + 0x1dd) = *(byte *)(_DAT_800ac784 + 0x1dd) | 1;
    *(undefined2 *)(_DAT_800ac784 + 0x1d4) = 0xaa10;
    *(undefined2 *)(_DAT_800ac784 + 0x1d6) = 0xd120;
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x60
                 );
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  if (*(uint *)(_DAT_800ac784 + 0x1d0) < 500) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 2;
  }
  FUN_8011bdd8(1,1);
  iVar2 = (uint)*(ushort *)(_DAT_800ac784 + 0x8c) << 0x10;
  *(short *)(_DAT_800ac784 + 0x8c) = (short)((iVar2 >> 0x10) - (iVar2 >> 0x1f) >> 1);
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0xa0;
  func_0x800245d8(0);
  return;
}


