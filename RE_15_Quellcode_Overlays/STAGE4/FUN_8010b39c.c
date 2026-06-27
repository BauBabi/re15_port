/* FUN_8010b39c @ 0x8010b39c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b39c(void)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 6);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined1 *)(_DAT_800ac784 + 6) = 3;
        *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x16;
        *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
        *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
        *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0x474;
        *(ushort *)(_DAT_800ac784 + 0x6a) = (*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4;
      }
      else if (bVar1 != 3) goto LAB_8010bdec;
      if (*(char *)(_DAT_800ac784 + 0x95) == 'F') {
        *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0x474;
        *(short *)(_DAT_800ac784 + 0x1ba) = *(short *)(_DAT_800ac784 + 0x1ba) + -0x708;
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar3 == 0) {
        return;
      }
      *(char *)(_DAT_800ac784 + 0x82) = *(char *)(_DAT_800ac784 + 0x82) + '\x01';
      *(int *)(_DAT_800ac784 + 0x38) = *(int *)(_DAT_800ac784 + 0x38) + -0x708;
      *(undefined1 *)(_DAT_800ac784 + 5) = 0;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined2 *)(_DAT_800ac784 + 0xb0) = 0;
      func_0x800245d8(0);
      return;
    }
    if (bVar1 != 0) goto LAB_8010bdec;
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x96) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  }
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  sVar2 = func_0x8001aa68((*(byte *)(_DAT_800ac784 + 0x90) & 0xf0) << 4,0x10);
  *(short *)(_DAT_800ac784 + 0x6a) = sVar2 + *(short *)(_DAT_800ac784 + 0x6a);
  if (sVar2 != 0) {
    return;
  }
  *(undefined1 *)(_DAT_800ac784 + 6) = 2;
LAB_8010bdec:
  *(undefined2 *)(_DAT_800ac784 + 0x166) = 0x14cc;
  *(undefined4 *)(_DAT_800ac784 + 0x78) =
       *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e4) * 4 + -0x7fee618c);
  uVar4 = (uint)*(ushort *)(*(int *)(_DAT_800ac784 + 0x78) + 6);
  func_0x8001af5c(0,0,uVar4 + 100,uVar4 + 200,_DAT_800ac784 + 0xb0,0x808080);
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(int *)(iVar3 + 0xc84) = iVar3 + 0xec;
  *(int *)(iVar3 + 0xca8) = iVar3 + 0xac;
  *(undefined4 *)(iVar3 + 0xc44) = 0x66;
  *(undefined4 *)(iVar3 + 0xc48) = 0xfffffcd6;
  *(undefined4 *)(iVar3 + 0xc4c) = 0;
  *(undefined2 *)(iVar3 + 0xc78) = 0;
  *(undefined2 *)(iVar3 + 0xc7a) = 0;
  *(undefined2 *)(iVar3 + 0xc7c) = 0;
  func_0x80068098(iVar3 + 0xc78,iVar3 + 0xc30);
  return;
}


