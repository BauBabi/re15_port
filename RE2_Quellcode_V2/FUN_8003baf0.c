/* FUN_8003baf0 @ 0x8003baf0  (Ghidra headless, raw MIPS overlay) */

void FUN_8003baf0(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  char cVar4;
  undefined *puVar5;
  undefined *unaff_s4;
  
  FUN_80012fb8(*(undefined2 *)(&DAT_800a3fe4 + (uint)*(byte *)(param_1 + 8) * 2),&DAT_801bfa14,1,
               &DAT_80010ec8);
  iVar3 = DAT_801bfa14;
  *(undefined **)(param_1 + 0x17c) = &DAT_801cd414 + *(int *)((int)&DAT_801bfa14 + DAT_801bfa14);
  iVar1 = *(int *)(&DAT_801bfa18 + iVar3);
  *(undefined1 *)(param_1 + 0x105) = 1;
  *(undefined1 *)(param_1 + 0x104) = 0x17;
  *(undefined **)(param_1 + 0x108) = &DAT_801cd414 + iVar1;
  *(undefined **)(param_1 + 0x14) = &DAT_801cd414 + *(int *)(&DAT_801bfa1c + iVar3);
  DAT_800cfbf0._1_1_ = 1;
  DAT_800cfbf0._0_1_ = 0x17;
  FUN_80076a40((int)&DAT_801bfa14 + *(int *)(&DAT_801bfa20 + iVar3));
  DrawSync(0);
  FUN_80076a00(&DAT_801cd414,&DAT_801bfa14,*(undefined4 *)(&DAT_801bfa20 + iVar3));
  FUN_80076b60(1,*(undefined4 *)(param_1 + 0x14),0x17,1);
  iVar1 = *(int *)(param_1 + 0x14);
  *(int *)(param_1 + 0x14) = iVar1 + 0xc;
  *(int *)(param_1 + 0x1c) = iVar1 + 0x28;
  if ((*(char *)(param_1 + 8) == '\x0e') ||
     (puVar5 = &DAT_801cd414 + *(int *)(&DAT_801bfa20 + iVar3), *(char *)(param_1 + 8) == '\n')) {
    puVar5 = &UNK_801cfc14 + *(int *)(&DAT_801bfa20 + iVar3);
    unaff_s4 = &DAT_801cd414 + *(int *)(&DAT_801bfa20 + iVar3);
  }
  DAT_800ce31c = FUN_80028324(param_1,puVar5);
  uVar2 = FUN_80028368(param_1,&DAT_801bfa14,*(undefined4 *)(param_1 + 0x108),1);
  cVar4 = *(char *)(param_1 + 8);
  if (cVar4 == '\x0e') {
    FUN_80019024(param_1,unaff_s4);
    cVar4 = *(char *)(param_1 + 8);
  }
  if (cVar4 == '\n') {
    FUN_80019440(param_1,unaff_s4);
  }
  DAT_800ce318 = 0xd2706ca4;
  iVar3 = *(int *)(param_1 + 0x198);
  DAT_800d7840 = *(undefined4 *)(iVar3 + 0x76c);
  DAT_800d8cec = *(undefined4 *)(iVar3 + 0x774);
  DAT_800d7660 = *(undefined4 *)(iVar3 + 0x770);
  DAT_800d7858 = *(undefined4 *)(iVar3 + 0x778);
  DAT_800ce320 = uVar2;
  *(undefined2 *)(param_1 + 0x162) = 200;
  if (*(char *)(param_1 + 8) == '\r') {
    *(undefined2 *)(param_1 + 0x162) = 400;
  }
  *(undefined2 *)(param_1 + 0x154) = 0;
  *(undefined2 *)(param_1 + 0x1de) = 0x1000;
  if (DAT_800d5bf8 == -0x80) {
    *(undefined2 *)(param_1 + 0x10e) = 0;
  }
  else {
    *(ushort *)(param_1 + 0x10e) = (ushort)DAT_800d5bfa;
  }
  FUN_800408c8(param_1);
  *(undefined1 *)(param_1 + 9) = 0x80;
  DAT_800cfd04 = 0;
  DAT_800d482c = (ushort)*(byte *)(param_1 + 8);
  if ((DAT_800cfb74 & 0x10000000) == 0) {
    FUN_80059c74(*(ushort *)(param_1 + 0x10e) & 0xfff);
  }
  else {
    FUN_80077708();
    FUN_80059c74(*(ushort *)(param_1 + 0x10e) & 0xfff);
    FUN_8007774c();
  }
  *(undefined2 *)(param_1 + 0x9a) = 0x1c2;
  *(undefined2 *)(param_1 + 0x9c) = 0x1c2;
  *(undefined2 *)(param_1 + 0x90) = 0x1c2;
  *(undefined2 *)(param_1 + 0x92) = 0x1c2;
  *(undefined4 *)(param_1 + 0x1e8) = 1;
  *(undefined2 *)(param_1 + 0x98) = 0xfa06;
  *(undefined2 *)(param_1 + 0x9e) = 0x5fa;
  *(undefined1 **)(param_1 + 0x204) = &LAB_8003bed8;
  *(undefined1 **)(param_1 + 0x208) = &LAB_8003bf7c;
  *(undefined2 *)(param_1 + 0x94) = 0;
  *(undefined2 *)(param_1 + 0x96) = 0;
  if ((*(char *)(param_1 + 8) == '\x0e') || (*(char *)(param_1 + 8) == '\n')) {
    **(uint **)(param_1 + 0x198) = **(uint **)(param_1 + 0x198) | 0x4000;
  }
  DAT_800a4005 = 0;
  if (((*(char *)(param_1 + 8) == '\0') || (*(char *)(param_1 + 8) == '\x02')) &&
     (iVar3 = FUN_80077360(&DAT_800d4854,0x3e), iVar3 != 0)) {
    FUN_80017054(*(int *)(param_1 + 0x198) + 4,*(byte *)(param_1 + 0x104) + 2,0,
                 *(byte *)(param_1 + 0x105) + 0x1e2);
  }
  iVar3 = FUN_80069f54(*(ushort *)(param_1 + 0x10e) & 0xfff);
  if (iVar3 != 0) {
    *(ushort *)(param_1 + 0x154) = *(ushort *)(param_1 + 0x154) | 1;
  }
  return;
}


