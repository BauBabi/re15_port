void FUN_80015a80(void)

{
  short sVar1;
  short sVar2;
  u_short uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  
  uVar7 = 0;
  iVar8 = 0;
  do {
    SetPolyGT4((POLY_GT4 *)(&DAT_800b8ab0 + iVar8));
    uVar3 = GetTPage(1,1,0x140,0x100);
    uVar4 = uVar7 & 1;
    uVar5 = uVar7 & 2;
    iVar6 = uVar7 << 8;
    uVar7 = uVar7 + 1;
    *(u_short *)((int)&DAT_800b8aca + iVar8) = uVar3;
    *(undefined2 *)((int)&DAT_800b8abe + iVar8) = 0x7fc0;
    *(undefined2 *)((int)&DAT_800b8aba + iVar8) = 0x58;
    *(undefined2 *)((int)&DAT_800b8ac6 + iVar8) = 0x58;
    *(uint *)((int)&DAT_800b8ae4 + iVar8) = (uVar4 * 0x20 + 0x10) * 0x10000;
    sVar1 = *(short *)((int)&DAT_800b8ae4 + iVar8 + 2);
    *(undefined2 *)((int)&DAT_800b8ad2 + iVar8) = 0x98;
    *(undefined2 *)((int)&DAT_800b8ade + iVar8) = 0x98;
    *(uint *)((int)&DAT_800b8ae8 + iVar8) = (uVar5 * 0x10 + 0x48) * 0x10000;
    sVar2 = *(short *)((int)&DAT_800b8ae8 + iVar8 + 2);
    *(undefined4 *)((int)&DAT_800b8ab4 + iVar8) = 0x3e00001c;
    *(undefined4 *)((int)&DAT_800b8ac0 + iVar8) = 0x3e00001c;
    *(undefined4 *)((int)&DAT_800b8acc + iVar8) = 0x3e00001c;
    *(undefined4 *)((int)&DAT_800b8ad8 + iVar8) = 0x3e00001c;
    *(undefined2 *)((int)&DAT_800b8ab8 + iVar8) = 0x20;
    *(undefined2 *)((int)&DAT_800b8ac4 + iVar8) = 0x120;
    *(undefined2 *)((int)&DAT_800b8ad0 + iVar8) = 0x20;
    *(undefined2 *)((int)&DAT_800b8adc + iVar8) = 0x120;
    (&DAT_800b8abc)[iVar8] = 0;
    (&DAT_800b8abd)[iVar8] = 0;
    (&DAT_800b8ac8)[iVar8] = 0xff;
    (&DAT_800b8ac9)[iVar8] = 0;
    (&DAT_800b8ad4)[iVar8] = 0;
    (&DAT_800b8ae0)[iVar8] = 0xff;
    *(short *)((int)&DAT_800b8ab8 + iVar8) = sVar1;
    *(short *)((int)&DAT_800b8aba + iVar8) = sVar2;
    *(short *)((int)&DAT_800b8ac4 + iVar8) = sVar1 + 0x100;
    (&DAT_800b8ad5)[iVar8] = 0x3f;
    (&DAT_800b8ae1)[iVar8] = 0x3f;
    *(int *)((int)&DAT_800b8aec + iVar8) = (0x200000 - *(int *)((int)&DAT_800b8ae4 + iVar8)) / 0x32;
    *(int *)((int)&DAT_800b8af0 + iVar8) = (0x580000 - *(int *)((int)&DAT_800b8ae8 + iVar8)) / 0x32;
    *(short *)((int)&DAT_800b8ac6 + iVar8) = sVar2;
    *(short *)((int)&DAT_800b8ad0 + iVar8) = sVar1;
    *(short *)((int)&DAT_800b8ad2 + iVar8) = sVar2 + 0x40;
    *(short *)((int)&DAT_800b8adc + iVar8) = sVar1 + 0x100;
    *(short *)((int)&DAT_800b8ade + iVar8) = sVar2 + 0x40;
    *(int *)((int)&DAT_800b8af4 + iVar8) = iVar6;
    iVar8 = iVar8 + 0x48;
  } while ((int)uVar7 < 4);
  uVar7 = 0;
  iVar8 = 0;
  do {
    SetPolyGT4((POLY_GT4 *)(&DAT_800b898c + iVar8));
    SetSemiTrans((POLY_GT4 *)(&DAT_800b898c + iVar8),0);
    uVar3 = GetTPage(1,1,0x140,0x100);
    *(u_short *)((int)&DAT_800b89a6 + iVar8) = uVar3;
    uVar3 = GetClut(0,0x1ff);
    *(u_short *)((int)&DAT_800b899a + iVar8) = uVar3;
    (&DAT_800b8990)[iVar8] = 0x80;
    (&DAT_800b8991)[iVar8] = 0x80;
    (&DAT_800b8992)[iVar8] = 0x80;
    (&DAT_800b899c)[iVar8] = 0x80;
    (&DAT_800b899d)[iVar8] = 0x80;
    (&DAT_800b899e)[iVar8] = 0x80;
    (&DAT_800b89a8)[iVar8] = 0x80;
    (&DAT_800b89a9)[iVar8] = 0x80;
    (&DAT_800b89aa)[iVar8] = 0x80;
    (&DAT_800b89b4)[iVar8] = 0x80;
    (&DAT_800b89b5)[iVar8] = 0x80;
    (&DAT_800b89b6)[iVar8] = 0x80;
    if ((uVar7 & 1) == 0) {
      *(undefined2 *)((int)&DAT_800b8994 + iVar8) = 0;
    }
    else {
      *(undefined2 *)((int)&DAT_800b8994 + iVar8) = 0x140;
    }
    if ((uVar7 & 2) == 0) {
      *(undefined2 *)((int)&DAT_800b8996 + iVar8) = 0;
    }
    else {
      *(undefined2 *)((int)&DAT_800b8996 + iVar8) = 0xf0;
    }
    *(undefined2 *)((int)&DAT_800b89a0 + iVar8) = 0xa0;
    if ((uVar7 & 2) == 0) {
      *(undefined2 *)((int)&DAT_800b89a2 + iVar8) = 0;
    }
    else {
      *(undefined2 *)((int)&DAT_800b89a2 + iVar8) = 0xf0;
    }
    if ((uVar7 & 1) == 0) {
      *(undefined2 *)((int)&DAT_800b89ac + iVar8) = 0;
    }
    else {
      *(undefined2 *)((int)&DAT_800b89ac + iVar8) = 0x140;
    }
    uVar7 = uVar7 + 1;
    *(undefined2 *)((int)&DAT_800b89b8 + iVar8) = 0xa0;
    (&DAT_800b8999)[iVar8] = 0x40;
    (&DAT_800b89a5)[iVar8] = 0x40;
    (&DAT_800b89b1)[iVar8] = 0xb7;
    (&DAT_800b89bd)[iVar8] = 0xb7;
    *(undefined2 *)((int)&DAT_800b89ae + iVar8) = 0x78;
    *(undefined2 *)((int)&DAT_800b89ba + iVar8) = 0x78;
    (&DAT_800b8998)[iVar8] = 0;
    (&DAT_800b89a4)[iVar8] = 0x9f;
    (&DAT_800b89b0)[iVar8] = 0;
    (&DAT_800b89bc)[iVar8] = 0x9f;
    iVar8 = iVar8 + 0x48;
  } while ((int)uVar7 < 4);
  return;
}
