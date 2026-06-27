/* FUN_8010bf1c @ 0x8010bf1c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010bf1c(undefined4 param_1,undefined4 param_2)

{
  bool bVar1;
  int in_v1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iStack00000010;
  
  iStack00000010 = _DAT_800ac784 + 0xb0;
  func_0x8001af5c(param_1,param_2,*(ushort *)(in_v1 + 6) + 100,*(ushort *)(in_v1 + 10) + 200);
  if ((*(byte *)(_DAT_800ac784 + 9) & 1) != 0) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x1000001;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x1e0) = 0;
    *(uint *)(_DAT_800ac784 + 0x38) = (uint)*(byte *)(_DAT_800ac784 + 0x82) * -0x708;
    *(undefined2 *)(_DAT_800ac784 + 0x1ba) = *(undefined2 *)(_DAT_800ac784 + 0x38);
  }
  iVar3 = 0xe1c;
  iVar4 = 0x15;
  do {
    iVar2 = iVar3 + *(int *)(_DAT_800ac784 + 0x188);
    iVar3 = iVar3 + -0xac;
    *(int *)(iVar2 + 0x2c) = (*(int *)(iVar2 + 0x2c) * 5) / 2;
    *(int *)(iVar2 + 0x30) = (*(int *)(iVar2 + 0x30) * 5) / 2;
    bVar1 = iVar4 != 0;
    *(int *)(iVar2 + 0x34) = (*(int *)(iVar2 + 0x34) * 5) / 2;
    iVar4 = iVar4 + -1;
  } while (bVar1);
  return;
}


