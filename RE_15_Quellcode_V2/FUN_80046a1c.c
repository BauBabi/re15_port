int FUN_80046a1c(int param_1)

{
  undefined1 uVar1;
  u_short uVar2;
  u_short uVar3;
  uint uVar4;
  int iVar5;
  u_short *puVar6;
  ushort uVar7;
  ushort uVar8;
  int iVar9;
  
  uVar2 = GetClut(0,0x1e4);
  uVar8 = 0;
  puVar6 = (u_short *)(param_1 + 0xe);
  do {
    iVar9 = param_1;
    uVar4 = (uint)uVar8;
    iVar5 = uVar4 * 4;
    *(undefined1 *)((int)puVar6 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar6 + -7) = 0x66;
    *(undefined1 *)(puVar6 + -5) = 0x80;
    *(undefined1 *)((int)puVar6 + -9) = 0x80;
    *(undefined1 *)(puVar6 + -4) = 0x80;
    *(undefined1 *)(puVar6 + -1) = (&DAT_80076244)[iVar5];
    *(undefined1 *)((int)puVar6 + -1) = (&DAT_80076246)[iVar5];
    puVar6[-3] = (&DAT_80076274)[uVar4 * 2];
    uVar3 = (&DAT_80076276)[uVar4 * 2];
    puVar6[1] = 0x28;
    puVar6[2] = 0x1e;
    *puVar6 = uVar2;
    puVar6[-2] = uVar3;
    *(undefined1 *)((int)puVar6 + 9) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)((int)puVar6 + 0xd) = 0x66;
    *(undefined1 *)(puVar6 + 5) = 0x80;
    *(undefined1 *)((int)puVar6 + 0xb) = 0x80;
    *(undefined1 *)(puVar6 + 6) = 0x80;
    puVar6[7] = (&DAT_80076274)[uVar4 * 2];
    puVar6[8] = (&DAT_80076276)[uVar4 * 2];
    param_1 = iVar9 + 0x28;
    *(undefined1 *)(puVar6 + 9) = (&DAT_80076244)[iVar5];
    uVar1 = (&DAT_80076246)[iVar5];
    uVar8 = uVar8 + 1;
                    /* Possible PsyQ macro: setPolyF4() */
    puVar6[0xb] = 0x28;
    puVar6[0xc] = 0x1e;
    puVar6[10] = uVar2;
    *(undefined1 *)((int)puVar6 + 0x13) = uVar1;
    puVar6 = puVar6 + 0x14;
  } while (uVar8 < 10);
  if ((char)DAT_800b25c0 == '\0') {
    *(undefined1 *)(iVar9 + 0x2b) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)(iVar9 + 0x2f) = 0x66;
    *(undefined1 *)(iVar9 + 0x2c) = 0x80;
    *(undefined1 *)(iVar9 + 0x2d) = 0x80;
    *(undefined1 *)(iVar9 + 0x2e) = 0x80;
    *(undefined1 *)(iVar9 + 0x34) = 0x28;
    *(undefined2 *)(iVar9 + 0x30) = 0x7e;
    *(undefined2 *)(iVar9 + 0x32) = 0x1a;
    *(undefined2 *)(iVar9 + 0x38) = 0x28;
    *(undefined2 *)(iVar9 + 0x3a) = 0x1e;
    *(u_short *)(iVar9 + 0x36) = uVar2;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
    *(undefined1 *)(iVar9 + 0x35) = 0x5a;
    *(undefined1 *)(iVar9 + 0x3f) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)(iVar9 + 0x43) = 0x66;
    *(undefined1 *)(iVar9 + 0x40) = 0x80;
    *(undefined1 *)(iVar9 + 0x41) = 0x80;
    *(undefined1 *)(iVar9 + 0x42) = 0x80;
    *(undefined1 *)(iVar9 + 0x48) = 0x28;
    *(undefined2 *)(iVar9 + 0x44) = 0x7e;
    *(undefined2 *)(iVar9 + 0x46) = 0x1a;
    *(undefined2 *)(iVar9 + 0x4c) = 0x28;
    *(undefined2 *)(iVar9 + 0x4e) = 0x1e;
    *(u_short *)(iVar9 + 0x4a) = uVar2;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
    *(undefined1 *)(iVar9 + 0x49) = 0x5a;
    param_1 = iVar9 + 0x50;
  }
  *(undefined1 *)(param_1 + 3) = 4;
                    /* Probable PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
  *(undefined1 *)(param_1 + 7) = 0x66;
  *(undefined1 *)(param_1 + 4) = 0x80;
  *(undefined1 *)(param_1 + 5) = 0x80;
  *(undefined1 *)(param_1 + 6) = 0x80;
  *(undefined1 *)(param_1 + 0xc) = 0x28;
  *(undefined2 *)(param_1 + 8) = 0x7e;
  *(undefined2 *)(param_1 + 10) = 0x1a;
  *(undefined2 *)(param_1 + 0x10) = 0x28;
  *(undefined2 *)(param_1 + 0x12) = 0x1e;
  *(u_short *)(param_1 + 0xe) = uVar2;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
  *(undefined1 *)(param_1 + 0xd) = 0x5a;
  *(undefined1 *)(param_1 + 0x17) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
  *(undefined1 *)(param_1 + 0x1b) = 0x66;
  *(undefined1 *)(param_1 + 0x18) = 0x80;
  *(undefined1 *)(param_1 + 0x19) = 0x80;
  *(undefined1 *)(param_1 + 0x1a) = 0x80;
  *(undefined1 *)(param_1 + 0x20) = 0x28;
  *(undefined2 *)(param_1 + 0x1c) = 0x7e;
  *(undefined2 *)(param_1 + 0x1e) = 0x1a;
  *(undefined2 *)(param_1 + 0x24) = 0x28;
  *(undefined2 *)(param_1 + 0x26) = 0x1e;
  *(u_short *)(param_1 + 0x22) = uVar2;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
  *(undefined1 *)(param_1 + 0x21) = 0x5a;
  *(undefined1 *)(param_1 + 0x2b) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
  *(undefined1 *)(param_1 + 0x2f) = 0x66;
  *(undefined1 *)(param_1 + 0x2c) = 0x80;
  *(undefined1 *)(param_1 + 0x2d) = 0x80;
  *(undefined1 *)(param_1 + 0x2e) = 0x80;
  *(undefined1 *)(param_1 + 0x34) = 0x28;
  *(undefined2 *)(param_1 + 0x30) = 0x7e;
  *(undefined2 *)(param_1 + 0x32) = 0x1a;
  *(undefined2 *)(param_1 + 0x38) = 0x28;
  *(undefined2 *)(param_1 + 0x3a) = 0x1e;
  *(u_short *)(param_1 + 0x36) = uVar2;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
  *(undefined1 *)(param_1 + 0x35) = 0x5a;
  *(undefined1 *)(param_1 + 0x3f) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
  *(undefined1 *)(param_1 + 0x43) = 0x66;
  *(undefined1 *)(param_1 + 0x40) = 0x80;
  *(undefined1 *)(param_1 + 0x41) = 0x80;
  *(undefined1 *)(param_1 + 0x42) = 0x80;
  *(undefined1 *)(param_1 + 0x48) = 0x28;
  *(undefined2 *)(param_1 + 0x44) = 0x7e;
  *(undefined2 *)(param_1 + 0x46) = 0x1a;
  *(undefined2 *)(param_1 + 0x4c) = 0x28;
  *(undefined2 *)(param_1 + 0x4e) = 0x1e;
  *(u_short *)(param_1 + 0x4a) = uVar2;
                    /* Possible PsyQ macro: setLineG3() + setSemiTrans(lineG3, 1) */
  *(undefined1 *)(param_1 + 0x49) = 0x5a;
  iVar5 = param_1 + 0x50;
  if ((char)DAT_800b25c0 == '\0') {
    *(undefined1 *)(param_1 + 0x53) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)(param_1 + 0x57) = 0x66;
    *(undefined1 *)(param_1 + 0x54) = 0x80;
    *(undefined1 *)(param_1 + 0x55) = 0x80;
    *(undefined1 *)(param_1 + 0x56) = 0x80;
    *(undefined1 *)(param_1 + 0x5c) = 0;
                    /* Possible PsyQ macro: setTile8() */
    *(undefined1 *)(param_1 + 0x5d) = 0x78;
    *(undefined2 *)(param_1 + 0x58) = 0;
    *(undefined2 *)(param_1 + 0x5a) = 0x96;
    *(undefined2 *)(param_1 + 0x60) = 0x50;
    *(undefined2 *)(param_1 + 0x62) = 0x1e;
    *(u_short *)(param_1 + 0x5e) = uVar2;
    *(undefined1 *)(param_1 + 0x67) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)(param_1 + 0x6b) = 0x66;
    *(undefined1 *)(param_1 + 0x68) = 0x80;
    *(undefined1 *)(param_1 + 0x69) = 0x80;
    *(undefined1 *)(param_1 + 0x6a) = 0x80;
    *(undefined1 *)(param_1 + 0x70) = 0;
                    /* Possible PsyQ macro: setTile8() */
    *(undefined1 *)(param_1 + 0x71) = 0x78;
    *(undefined2 *)(param_1 + 0x6c) = 0;
    *(undefined2 *)(param_1 + 0x6e) = 0x96;
    *(undefined2 *)(param_1 + 0x74) = 0x50;
    *(undefined2 *)(param_1 + 0x76) = 0x1e;
    *(u_short *)(param_1 + 0x72) = uVar2;
    uVar2 = GetClut(0,0x1e5);
    *(undefined1 *)(param_1 + 0x7b) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)(param_1 + 0x7f) = 0x66;
    *(undefined1 *)(param_1 + 0x7c) = 0x80;
    *(undefined1 *)(param_1 + 0x7d) = 0x80;
    *(undefined1 *)(param_1 + 0x7e) = 0x80;
    *(undefined1 *)(param_1 + 0x84) = 0;
    *(undefined1 *)(param_1 + 0x85) = 0x96;
    *(undefined2 *)(param_1 + 0x80) = 0;
    *(undefined2 *)(param_1 + 0x82) = 8;
    *(undefined2 *)(param_1 + 0x88) = 0x80;
    *(undefined2 *)(param_1 + 0x8a) = 0x3f;
    *(u_short *)(param_1 + 0x86) = uVar2;
    *(undefined1 *)(param_1 + 0x8f) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
    *(undefined1 *)(param_1 + 0x93) = 0x66;
    *(undefined1 *)(param_1 + 0x90) = 0x80;
    *(undefined1 *)(param_1 + 0x91) = 0x80;
    *(undefined1 *)(param_1 + 0x92) = 0x80;
    *(undefined1 *)(param_1 + 0x98) = 0;
    *(undefined1 *)(param_1 + 0x99) = 0x96;
    *(undefined2 *)(param_1 + 0x94) = 0;
    *(undefined2 *)(param_1 + 0x96) = 8;
    *(undefined2 *)(param_1 + 0x9c) = 0x80;
    *(undefined2 *)(param_1 + 0x9e) = 0x3f;
    *(u_short *)(param_1 + 0x9a) = uVar2;
    iVar5 = param_1 + 0xa0;
    uVar8 = 0;
    do {
      uVar7 = 0;
      puVar6 = (u_short *)(iVar5 + 0xe);
      do {
        uVar3 = uVar7 * 0x28 + 0xc;
        *(undefined1 *)((int)puVar6 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
        *(undefined1 *)((int)puVar6 + -7) = 0x66;
        *(undefined1 *)(puVar6 + -5) = 0x80;
        *(undefined1 *)((int)puVar6 + -9) = 0x80;
        *(undefined1 *)(puVar6 + -4) = 0x80;
        *(undefined1 *)(puVar6 + -1) = 0;
        *(undefined1 *)((int)puVar6 + -1) = 0xd5;
        puVar6[-3] = uVar8 * 0x28;
        puVar6[-2] = uVar3;
        puVar6[1] = 0x28;
        puVar6[2] = 0x28;
        *puVar6 = uVar2;
        *(undefined1 *)((int)puVar6 + 9) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
        *(undefined1 *)((int)puVar6 + 0xd) = 0x66;
        *(undefined1 *)(puVar6 + 5) = 0x80;
        *(undefined1 *)((int)puVar6 + 0xb) = 0x80;
        *(undefined1 *)(puVar6 + 6) = 0x80;
        *(undefined1 *)(puVar6 + 9) = 0;
        *(undefined1 *)((int)puVar6 + 0x13) = 0xd5;
        puVar6[7] = uVar8 * 0x28;
        puVar6[8] = uVar3;
        puVar6[0xb] = 0x28;
        puVar6[0xc] = 0x28;
        puVar6[10] = uVar2;
        puVar6 = puVar6 + 0x14;
        uVar7 = uVar7 + 1;
        iVar5 = iVar5 + 0x28;
      } while (uVar7 < 6);
      uVar8 = uVar8 + 1;
    } while (uVar8 < 8);
  }
  return iVar5;
}
