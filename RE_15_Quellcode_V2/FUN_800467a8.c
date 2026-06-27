undefined * FUN_800467a8(uint param_1)

{
  ushort uVar1;
  int iVar2;
  undefined2 *puVar3;
  undefined2 *puVar4;
  undefined *puVar5;
  undefined2 *puVar6;
  ushort uVar7;
  
  param_1 = param_1 & 0xff;
  iVar2 = param_1 * 0xc;
  puVar6 = (undefined2 *)(&PTR_DAT_80074a90)[param_1 * 3];
  uVar1 = *(ushort *)(&DAT_80074a8e + iVar2);
  puVar5 = (&PTR_DAT_80074a94)[param_1 * 3];
  uVar7 = 0;
  if (uVar1 != 0) {
    puVar4 = puVar6 + 1;
    puVar3 = (undefined2 *)(puVar5 + 0xe);
    do {
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar3 + -7) = 100;
      *(undefined1 *)((int)puVar3 + -0xb) = 4;
      *(undefined1 *)(puVar3 + -5) = 0x80;
      *(undefined1 *)((int)puVar3 + -9) = 0x80;
      *(undefined1 *)(puVar3 + -4) = 0x80;
      *(byte *)((int)puVar3 + -7) = *(byte *)((int)puVar3 + -7) & 0xfd;
      puVar3[1] = puVar4[1];
      puVar3[2] = puVar4[2];
      *(undefined1 *)(puVar3 + -1) = *(undefined1 *)(puVar4 + 3);
      *(undefined1 *)((int)puVar3 + -1) = *(undefined1 *)(puVar4 + 4);
      puVar3[-3] = *puVar6;
      puVar3[-2] = *puVar4;
      *puVar3 = (&DAT_800b2610)[*(ushort *)(&DAT_80074a8c + iVar2)];
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar3 + 0xd) = 100;
      *(undefined1 *)((int)puVar3 + 9) = 4;
      *(undefined1 *)(puVar3 + 5) = 0x80;
      *(undefined1 *)((int)puVar3 + 0xb) = 0x80;
      *(undefined1 *)(puVar3 + 6) = 0x80;
      *(byte *)((int)puVar3 + 0xd) = *(byte *)((int)puVar3 + 0xd) & 0xfd;
      puVar3[0xb] = puVar4[1];
      puVar3[0xc] = puVar4[2];
      *(undefined1 *)(puVar3 + 9) = *(undefined1 *)(puVar4 + 3);
      *(undefined1 *)((int)puVar3 + 0x13) = *(undefined1 *)(puVar4 + 4);
      puVar5 = puVar5 + 0x28;
      puVar3[7] = *puVar6;
      uVar7 = uVar7 + 1;
      puVar3[8] = *puVar4;
      puVar6 = puVar6 + 6;
      puVar4 = puVar4 + 6;
      puVar3[10] = (&DAT_800b2610)[*(ushort *)(&DAT_80074a8c + iVar2)];
      puVar3 = puVar3 + 0x14;
    } while (uVar7 < uVar1);
  }
  return puVar5;
}
