/* FUN_80013c1c @ 0x80013c1c  (Ghidra headless, raw MIPS overlay) */

void FUN_80013c1c(void)

{
  int *piVar1;
  undefined2 *puVar2;
  undefined *puVar3;
  int iVar4;
  undefined4 uVar5;
  int iVar6;
  undefined4 *puVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  
  local_24 = DAT_80010830;
  local_20 = DAT_80010834;
  local_1c = DAT_80010838;
  local_18 = DAT_8001083c;
  local_14 = DAT_80010840;
  local_10 = DAT_80010844;
  local_c = DAT_80010848;
  DAT_800c3a84 = 1;
  uVar9 = DAT_8001082c;
  FUN_80015064();
  iVar4 = 9;
  puVar7 = &DAT_800d4dfc;
  puVar3 = &DAT_801bddf8;
  DAT_800c3a80 = &DAT_801bd000;
  DAT_800cfb74 = DAT_800cfb74 | 0x2000000;
  do {
    *puVar7 = puVar3;
    puVar7 = puVar7 + -1;
    iVar4 = iVar4 + -1;
    puVar3 = puVar3 + -0x14c;
  } while (-1 < iVar4);
  puVar2 = DAT_800d4dd8;
  if (DAT_800d4dd8 <= (undefined2 *)(DAT_800d4dfc + 0x146)) {
    do {
      *puVar2 = 0;
      puVar2 = puVar2 + 1;
    } while (puVar2 <= (undefined2 *)(DAT_800d4dfc + 0x146));
  }
  DAT_800cfbf0 = 0x1f15;
  DAT_801a1004 = (int)&DAT_801a1000 + DAT_801a1004;
  DAT_801a1000 = (int)&DAT_801a1000 + DAT_801a1000;
  *DAT_800c3a80 = DAT_801a1004;
  FUN_80076a40();
  iVar4 = DAT_801a1000;
  DAT_800c3a80[1] = DAT_801a1000;
  if (*(char *)(DAT_800ce550 + 0xc) == '(') {
    uVar5 = 0;
    uVar8 = 0;
  }
  else {
    uVar5 = 0x15;
    uVar8 = 0x1f;
  }
  FUN_80076b60(1,iVar4,uVar5,uVar8,uVar9);
  piVar1 = DAT_800c3a80;
  iVar6 = 10;
  puVar3 = &DAT_800d86e8;
  iVar4 = DAT_800c3a80[1];
  DAT_800c3a84 = 0;
  DAT_800c3a80[3] = 3;
  piVar1[4] = 0x140;
  piVar1[5] = 0xf0;
  *(undefined2 *)((int)piVar1 + 0x22e) = 0;
  piVar1[1] = iVar4 + 0xc;
  do {
    puVar3[3] = (char)iVar6;
    puVar3[1] = 0;
    puVar3[2] = 0;
    puVar3[4] = 0xff;
    puVar3[8] = 0xff;
    iVar6 = iVar6 + 1;
    puVar3 = puVar3 + 0x174;
  } while (iVar6 < 0xe);
  DAT_800d8cbc = &DAT_801a1008;
  FUN_800530ec(&DAT_800d86e8,0);
  FUN_8008de24(0x122);
  FUN_80076cb0(&local_24,&local_18);
  FUN_8002bda8(2,0);
  DAT_800d4806 = 1;
  DAT_800d4804 = *(byte *)(DAT_800ce550 + 0xd) & 0xff7f;
  DAT_800d4808 = *(byte *)(DAT_800ce550 + 0xd) & 0x80;
  DAT_800d480a = (ushort)*(byte *)(DAT_800ce550 + 0xc);
  *(undefined2 *)(DAT_800c3a80 + 0x92) = 0;
  return;
}


