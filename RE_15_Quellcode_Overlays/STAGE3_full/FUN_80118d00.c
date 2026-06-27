/* FUN_80118d00 @ 0x80118d00  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80118d00(void)

{
  char cVar1;
  int iVar2;
  undefined4 uVar3;
  int iVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  
  *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
  switch(*(undefined1 *)(_DAT_800ac784 + 6)) {
  case 0:
    *(undefined1 *)(_DAT_800ac784 + 6) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x800453d0(3);
    func_0x800453d0(6);
  case 1:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x100;
    break;
  case 2:
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 10;
    *(undefined1 *)(_DAT_800ac784 + 6) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x800453d0(8);
  case 3:
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x5de) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x5de) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x5de) = 0;
    }
    func_0x8003a95c(iVar4 + 0x560,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x68a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x68a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x68a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x60c,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x93a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x93a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x93a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x8bc,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x9e6) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x9e6) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x9e6) = 0;
    }
    func_0x8003a95c(iVar4 + 0x968,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0xa92) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0xa92) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0xa92) = 0;
    }
    func_0x8003a95c(iVar4 + 0xa14,1);
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
    if (*(short *)(iVar4 + 0xa92) < 1) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    }
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x100;
    break;
  case 4:
    *(undefined1 *)(_DAT_800ac784 + 6) = 5;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x800453d0(9);
  case 5:
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x5de) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x5de) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x5de) = 0;
    }
    func_0x8003a95c(iVar4 + 0x560,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x68a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x68a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x68a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x60c,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x93a) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x93a) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x93a) = 0;
    }
    func_0x8003a95c(iVar4 + 0x8bc,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0x9e6) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0x9e6) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0x9e6) = 0;
    }
    func_0x8003a95c(iVar4 + 0x968,1);
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    iVar2 = (uint)*(ushort *)(iVar4 + 0xa92) - (uint)*(ushort *)(_DAT_800ac784 + 0x9c);
    *(short *)(iVar4 + 0xa92) = (short)iVar2;
    if (iVar2 * 0x10000 < 0) {
      *(undefined2 *)(iVar4 + 0xa92) = 0;
    }
    func_0x8003a95c(iVar4 + 0xa14,1);
    *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + 10;
    if (*(short *)(iVar4 + 0xa92) < 1) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 6;
    }
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x100);
    return;
  case 6:
    *(undefined1 *)(_DAT_800ac784 + 6) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    func_0x8004ef90(0x800b1028,0x1d);
  case 7:
    if (*(char *)(_DAT_800ac784 + 0x95) == '\x14') {
      func_0x800453d0(4);
    }
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x100;
    break;
  case 8:
    *(undefined1 *)(_DAT_800ac784 + 6) = 9;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 4;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x8004ef90(0x800b1028,0x1f);
    uVar3 = 0;
    goto LAB_80119258;
  case 9:
  case 0xb:
    goto switchD_80118d58_caseD_9;
  case 10:
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x800453d0(0);
    uVar3 = 1;
LAB_80119258:
    func_0x800453d0(uVar3);
switchD_80118d58_caseD_9:
    uVar3 = *(undefined4 *)(_DAT_800ac784 + 0x84);
    uVar5 = *(undefined4 *)(_DAT_800ac784 + 0x16c);
    uVar6 = 0x400;
    break;
  case 0xc:
    *(undefined1 *)(_DAT_800ac784 + 6) = 0xd;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x14;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 300;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 3;
    func_0x8004ef90(0x800b1028,0x1e);
    func_0x800453d0(6);
  case 0xd:
    cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                            *(undefined4 *)(_DAT_800ac784 + 0x16c),1,0x400);
    *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
    *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x8c) + 0x32;
    func_0x800245d8(0x800);
    return;
  default:
    goto switchD_80118d58_caseD_e;
  }
  cVar1 = func_0x8001f314(uVar3,uVar5,0,uVar6);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
switchD_80118d58_caseD_e:
  return;
}


