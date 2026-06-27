int FUN_8004694c(int param_1)

{
  ushort uVar1;
  undefined2 *puVar2;
  ushort uVar3;
  
  uVar1 = 0x1e;
  if ((char)DAT_800b25c0 == '\0') {
    uVar1 = 0x21;
  }
  uVar3 = 0;
  if (uVar1 != 0) {
    puVar2 = (undefined2 *)(param_1 + 0xe);
    do {
      *(undefined1 *)((int)puVar2 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar2 + -7) = 0x66;
      *(undefined1 *)(puVar2 + -5) = 0x80;
      *(undefined1 *)((int)puVar2 + -9) = 0x80;
      *(undefined1 *)(puVar2 + -4) = 0x80;
      puVar2[-3] = 0;
      puVar2[-2] = 0;
      *(undefined1 *)(puVar2 + -1) = 0;
      *(undefined1 *)((int)puVar2 + -1) = 0xf0;
      puVar2[1] = 8;
      puVar2[2] = 8;
      param_1 = param_1 + 0x28;
      *puVar2 = DAT_800b2614;
      *(undefined1 *)((int)puVar2 + 9) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar2 + 0xd) = 0x66;
      *(undefined1 *)(puVar2 + 5) = 0x80;
      *(undefined1 *)((int)puVar2 + 0xb) = 0x80;
      *(undefined1 *)(puVar2 + 6) = 0x80;
      puVar2[7] = 0;
      puVar2[8] = 0;
      *(undefined1 *)(puVar2 + 9) = 0;
      *(undefined1 *)((int)puVar2 + 0x13) = 0xf0;
      puVar2[0xb] = 8;
      puVar2[0xc] = 8;
      uVar3 = uVar3 + 1;
      puVar2[10] = DAT_800b2614;
      puVar2 = puVar2 + 0x14;
    } while (uVar3 < uVar1);
  }
  return param_1;
}
