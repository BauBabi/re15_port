void FUN_800297c8(void)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  
  iVar5 = 0;
  iVar4 = 0xc0;
  iVar3 = 0;
  do {
    *(undefined2 *)((int)&DAT_800b2924 + iVar3) = 0;
    iVar5 = iVar5 + 1;
    iVar1 = iVar4 + _DAT_00000110;
    *(int *)((int)&DAT_800b2930 + iVar3) = iVar1;
    *(undefined4 *)(iVar1 + 0x94) = 0x404;
    uVar2 = FUN_8006e4c8();
    iVar4 = iVar4 + 0xc0;
    *(undefined4 *)((int)&DAT_800b2968 + iVar3) = uVar2;
    iVar3 = iVar3 + 0x80;
  } while (iVar5 < 3);
  DAT_800b2934 = &DAT_801ff400;
  DAT_800b29b4 = &DAT_801ff700;
  DAT_800b2a34 = &DAT_801ffa00;
  DAT_800b2920 = FUN_8006e4c8();
  return;
}
