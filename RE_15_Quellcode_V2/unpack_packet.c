void unpack_packet(uint *param_1,uint *param_2)

{
  int iVar1;
  uint uVar2;
  
  memset((uchar *)param_2,'\0',0x78);
  uVar2 = *param_1 & 0xfdffffff;
  *param_2 = *param_1;
  if (uVar2 == 0x2d010709) {
    iVar1 = GetGraphDebug();
    if (iVar1 == 2) {
      printf("FT4 ");
    }
    *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
    *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
    *(char *)(param_2 + 6) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
    *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
    *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
    *(char *)(param_2 + 7) = (char)param_1[3];
    *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
    *(char *)((int)param_2 + 0x1e) = (char)param_1[4];
    *(undefined1 *)((int)param_2 + 0x1f) = *(undefined1 *)((int)param_1 + 0x11);
    *(char *)(param_2 + 1) = (char)param_1[5];
    *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 0x15);
    *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 0x16);
    *(char *)(param_2 + 2) = (char)param_1[5];
    *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 0x15);
    *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 0x16);
    *(char *)(param_2 + 3) = (char)param_1[5];
    *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 0x15);
    *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 0x16);
    *(char *)(param_2 + 4) = (char)param_1[5];
    *(undefined1 *)((int)param_2 + 0x11) = *(undefined1 *)((int)param_1 + 0x15);
    *(undefined1 *)((int)param_2 + 0x12) = *(undefined1 *)((int)param_1 + 0x16);
    *(short *)(param_2 + 0x1a) = (short)param_1[6];
    *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x1a);
    *(short *)(param_2 + 0x1b) = (short)param_1[7];
    *(undefined2 *)((int)param_2 + 0x6e) = *(undefined2 *)((int)param_1 + 0x1e);
    TMD_OBJ_1784();
    return;
  }
  if (0x2d010709 < uVar2) {
    if (uVar2 == 0x35010809) {
      iVar1 = GetGraphDebug();
      if (iVar1 == 2) {
        printf("GT3 ");
      }
      *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
      *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
      *(char *)(param_2 + 6) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
      *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
      *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
      *(char *)(param_2 + 7) = (char)param_1[3];
      *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
      *(short *)(param_2 + 0x1a) = (short)param_1[7];
      *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x1e);
      *(short *)(param_2 + 0x1b) = (short)param_1[8];
      *(char *)(param_2 + 1) = (char)param_1[4];
      *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 0x11);
      *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 0x12);
      *(char *)(param_2 + 2) = (char)param_1[5];
      *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 0x15);
      *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 0x16);
      *(char *)(param_2 + 3) = (char)param_1[6];
      *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 0x19);
      *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 0x1a);
      TMD_OBJ_1784();
      return;
    }
    if (uVar2 < 0x3501080a) {
      if (uVar2 == 0x31010506) {
        iVar1 = GetGraphDebug();
        if (iVar1 == 2) {
          printf("G3 ");
        }
        *(char *)(param_2 + 1) = (char)param_1[1];
        *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
        *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
        *(char *)(param_2 + 2) = (char)param_1[2];
        *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 9);
        *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 10);
        *(char *)(param_2 + 3) = (char)param_1[3];
        *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 0xd);
        *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 0xe);
        *(short *)(param_2 + 0x1a) = (short)param_1[4];
        *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x12);
        *(short *)(param_2 + 0x1b) = (short)param_1[5];
        TMD_OBJ_1784();
        return;
      }
      if (uVar2 < 0x31010507) {
        if (uVar2 != 0x30000406) {
          TMD_OBJ_1768();
          return;
        }
        iVar1 = GetGraphDebug();
        if (iVar1 == 2) {
          printf("G3L ");
        }
        *(char *)(param_2 + 1) = (char)param_1[1];
        *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
        *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
        *(char *)(param_2 + 2) = (char)param_1[1];
        *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 5);
        *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 6);
        *(char *)(param_2 + 3) = (char)param_1[1];
        *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 5);
        *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 6);
        *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 10);
        *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0xe);
        *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0x12);
        *(short *)(param_2 + 0x1c) = (short)param_1[2];
        *(short *)((int)param_2 + 0x72) = (short)param_1[3];
        *(short *)(param_2 + 0x1d) = (short)param_1[4];
        TMD_OBJ_1784();
        return;
      }
      if (uVar2 != 0x34000609) {
        TMD_OBJ_1768();
        return;
      }
      iVar1 = GetGraphDebug();
      if (iVar1 == 2) {
        printf("GT3L ");
      }
      *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
      *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
      *(char *)(param_2 + 6) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
      *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
      *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
      *(char *)(param_2 + 7) = (char)param_1[3];
      *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
      *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 0x12);
      *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x16);
      *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0x1a);
      *(short *)(param_2 + 0x1c) = (short)param_1[4];
      *(short *)((int)param_2 + 0x72) = (short)param_1[5];
      *(short *)(param_2 + 0x1d) = (short)param_1[6];
      TMD_OBJ_1784();
      return;
    }
    if (uVar2 == 0x39010608) {
      iVar1 = GetGraphDebug();
      if (iVar1 == 2) {
        printf("G4 ");
      }
      *(char *)(param_2 + 1) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 2) = (char)param_1[2];
      *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 9);
      *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 10);
      *(char *)(param_2 + 3) = (char)param_1[3];
      *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 0xd);
      *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 0xe);
      *(char *)(param_2 + 4) = (char)param_1[4];
      *(undefined1 *)((int)param_2 + 0x11) = *(undefined1 *)((int)param_1 + 0x11);
      *(undefined1 *)((int)param_2 + 0x12) = *(undefined1 *)((int)param_1 + 0x12);
      *(short *)(param_2 + 0x1a) = (short)param_1[5];
      *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x16);
      *(short *)(param_2 + 0x1b) = (short)param_1[6];
      *(undefined2 *)((int)param_2 + 0x6e) = *(undefined2 *)((int)param_1 + 0x1a);
      TMD_OBJ_1784();
      return;
    }
    if (uVar2 < 0x39010609) {
      if (uVar2 != 0x38000508) {
        TMD_OBJ_1768();
        return;
      }
      iVar1 = GetGraphDebug();
      if (iVar1 == 2) {
        printf("G4L ");
      }
      *(char *)(param_2 + 1) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 2) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 3) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 4) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0x11) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 0x12) = *(undefined1 *)((int)param_1 + 6);
      *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 10);
      *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0xe);
      *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0x12);
      *(undefined2 *)((int)param_2 + 0x6e) = *(undefined2 *)((int)param_1 + 0x16);
      *(short *)(param_2 + 0x1c) = (short)param_1[2];
      *(short *)((int)param_2 + 0x72) = (short)param_1[3];
      *(short *)(param_2 + 0x1d) = (short)param_1[4];
      *(short *)((int)param_2 + 0x76) = (short)param_1[5];
      TMD_OBJ_1784();
      return;
    }
    if (uVar2 == 0x3c00080c) {
      iVar1 = GetGraphDebug();
      if (iVar1 == 2) {
        printf("GT4L ");
      }
      *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
      *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
      *(char *)(param_2 + 6) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
      *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
      *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
      *(char *)(param_2 + 7) = (char)param_1[3];
      *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
      *(char *)((int)param_2 + 0x1e) = (char)param_1[4];
      *(undefined1 *)((int)param_2 + 0x1f) = *(undefined1 *)((int)param_1 + 0x11);
      *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 0x16);
      *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x1a);
      *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0x1e);
      *(undefined2 *)((int)param_2 + 0x6e) = *(undefined2 *)((int)param_1 + 0x22);
      *(short *)(param_2 + 0x1c) = (short)param_1[5];
      *(short *)((int)param_2 + 0x72) = (short)param_1[6];
      *(short *)(param_2 + 0x1d) = (short)param_1[7];
      *(short *)((int)param_2 + 0x76) = (short)param_1[8];
      TMD_OBJ_1784();
      return;
    }
    if (uVar2 != 0x3d010a0c) {
      TMD_OBJ_1768();
      return;
    }
    iVar1 = GetGraphDebug();
    if (iVar1 == 2) {
      printf("GT4 ");
    }
    *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
    *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
    *(char *)(param_2 + 6) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
    *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
    *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
    *(char *)(param_2 + 7) = (char)param_1[3];
    *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
    *(char *)((int)param_2 + 0x1e) = (char)param_1[4];
    *(undefined1 *)((int)param_2 + 0x1f) = *(undefined1 *)((int)param_1 + 0x11);
    *(short *)(param_2 + 0x1a) = (short)param_1[9];
    *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x26);
    *(short *)(param_2 + 0x1b) = (short)param_1[10];
    *(undefined2 *)((int)param_2 + 0x6e) = *(undefined2 *)((int)param_1 + 0x2a);
    *(char *)(param_2 + 1) = (char)param_1[5];
    *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 0x15);
    *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 0x16);
    *(char *)(param_2 + 2) = (char)param_1[6];
    *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 0x19);
    *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 0x1a);
    *(char *)(param_2 + 3) = (char)param_1[7];
    *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 0x1d);
    *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 0x1e);
    *(char *)(param_2 + 4) = (char)param_1[8];
    *(undefined1 *)((int)param_2 + 0x11) = *(undefined1 *)((int)param_1 + 0x21);
    *(undefined1 *)((int)param_2 + 0x12) = *(undefined1 *)((int)param_1 + 0x22);
    TMD_OBJ_1784();
    return;
  }
  if (uVar2 == 0x25010607) {
    iVar1 = GetGraphDebug();
    if (iVar1 == 2) {
      printf("FT3 ");
    }
    *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
    *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
    *(char *)(param_2 + 6) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
    *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
    *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
    *(char *)(param_2 + 7) = (char)param_1[3];
    *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
    *(char *)(param_2 + 1) = (char)param_1[4];
    *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 0x11);
    *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 0x12);
    *(char *)(param_2 + 2) = (char)param_1[4];
    *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 0x11);
    *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 0x12);
    *(char *)(param_2 + 3) = (char)param_1[4];
    *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 0x11);
    *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 0x12);
    *(short *)(param_2 + 0x1a) = (short)param_1[5];
    *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 0x16);
    *(short *)(param_2 + 0x1b) = (short)param_1[6];
    TMD_OBJ_1784();
    return;
  }
  if (0x25010607 < uVar2) {
    if (uVar2 == 0x29010305) {
      iVar1 = GetGraphDebug();
      if (iVar1 == 2) {
        printf("F4 ");
      }
      *(char *)(param_2 + 1) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 2) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 3) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 4) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0x11) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 0x12) = *(undefined1 *)((int)param_1 + 6);
      *(short *)(param_2 + 0x1a) = (short)param_1[2];
      *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 10);
      *(short *)(param_2 + 0x1b) = (short)param_1[3];
      *(undefined2 *)((int)param_2 + 0x6e) = *(undefined2 *)((int)param_1 + 0xe);
      TMD_OBJ_1784();
      return;
    }
    if (uVar2 < 0x29010306) {
      if (uVar2 != 0x28000405) {
        TMD_OBJ_1768();
        return;
      }
      iVar1 = GetGraphDebug();
      if (iVar1 == 2) {
        printf("F4L ");
      }
      *(char *)(param_2 + 1) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 2) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 3) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 6);
      *(char *)(param_2 + 4) = (char)param_1[1];
      *(undefined1 *)((int)param_2 + 0x11) = *(undefined1 *)((int)param_1 + 5);
      *(undefined1 *)((int)param_2 + 0x12) = *(undefined1 *)((int)param_1 + 6);
      *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 10);
      *(short *)((int)param_2 + 0x6a) = (short)param_1[3];
      *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0xe);
      *(short *)((int)param_2 + 0x6e) = (short)param_1[4];
      *(short *)(param_2 + 0x1c) = (short)param_1[2];
      *(short *)((int)param_2 + 0x72) = (short)param_1[2];
      *(short *)(param_2 + 0x1d) = (short)param_1[2];
      *(short *)((int)param_2 + 0x76) = (short)param_1[2];
      TMD_OBJ_1784();
      return;
    }
    if (uVar2 != 0x2c000709) {
      TMD_OBJ_1768();
      return;
    }
    iVar1 = GetGraphDebug();
    if (iVar1 == 2) {
      printf("FT4L ");
    }
    *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
    *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
    *(char *)(param_2 + 6) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
    *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
    *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
    *(char *)(param_2 + 7) = (char)param_1[3];
    *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
    *(char *)((int)param_2 + 0x1e) = (char)param_1[4];
    *(undefined1 *)((int)param_2 + 0x1f) = *(undefined1 *)((int)param_1 + 0x11);
    *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 0x16);
    *(short *)((int)param_2 + 0x6a) = (short)param_1[6];
    *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0x1a);
    *(short *)((int)param_2 + 0x6e) = (short)param_1[7];
    *(short *)(param_2 + 0x1c) = (short)param_1[5];
    *(short *)((int)param_2 + 0x72) = (short)param_1[5];
    *(short *)(param_2 + 0x1d) = (short)param_1[5];
    *(short *)((int)param_2 + 0x76) = (short)param_1[5];
    TMD_OBJ_1784();
    return;
  }
  if (uVar2 == 0x21010304) {
    iVar1 = GetGraphDebug();
    if (iVar1 == 2) {
      printf("F3 ");
    }
    *(char *)(param_2 + 1) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
    *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
    *(char *)(param_2 + 2) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 5);
    *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 6);
    *(char *)(param_2 + 3) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 5);
    *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 6);
    *(short *)(param_2 + 0x1a) = (short)param_1[2];
    *(undefined2 *)((int)param_2 + 0x6a) = *(undefined2 *)((int)param_1 + 10);
    *(short *)(param_2 + 0x1b) = (short)param_1[3];
    TMD_OBJ_1784();
    return;
  }
  if (uVar2 < 0x21010305) {
    if (uVar2 != 0x20000304) {
      TMD_OBJ_1768();
      return;
    }
    iVar1 = GetGraphDebug();
    if (iVar1 == 2) {
      printf("F3L ");
    }
    *(char *)(param_2 + 1) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 5) = *(undefined1 *)((int)param_1 + 5);
    *(undefined1 *)((int)param_2 + 6) = *(undefined1 *)((int)param_1 + 6);
    *(char *)(param_2 + 2) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 9) = *(undefined1 *)((int)param_1 + 5);
    *(undefined1 *)((int)param_2 + 10) = *(undefined1 *)((int)param_1 + 6);
    *(char *)(param_2 + 3) = (char)param_1[1];
    *(undefined1 *)((int)param_2 + 0xd) = *(undefined1 *)((int)param_1 + 5);
    *(undefined1 *)((int)param_2 + 0xe) = *(undefined1 *)((int)param_1 + 6);
    *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 10);
    *(short *)((int)param_2 + 0x6a) = (short)param_1[3];
    *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0xe);
    *(short *)(param_2 + 0x1c) = (short)param_1[2];
    *(short *)((int)param_2 + 0x72) = (short)param_1[2];
    *(short *)(param_2 + 0x1d) = (short)param_1[2];
    TMD_OBJ_1784();
    return;
  }
  if (uVar2 != 0x24000507) {
    TMD_OBJ_1768();
    return;
  }
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("FT3L ");
  }
  *(undefined2 *)(param_2 + 5) = *(undefined2 *)((int)param_1 + 10);
  *(undefined2 *)((int)param_2 + 0x16) = *(undefined2 *)((int)param_1 + 6);
  *(char *)(param_2 + 6) = (char)param_1[1];
  *(undefined1 *)((int)param_2 + 0x19) = *(undefined1 *)((int)param_1 + 5);
  *(char *)((int)param_2 + 0x1a) = (char)param_1[2];
  *(undefined1 *)((int)param_2 + 0x1b) = *(undefined1 *)((int)param_1 + 9);
  *(char *)(param_2 + 7) = (char)param_1[3];
  *(undefined1 *)((int)param_2 + 0x1d) = *(undefined1 *)((int)param_1 + 0xd);
  *(undefined2 *)(param_2 + 0x1a) = *(undefined2 *)((int)param_1 + 0x12);
  *(short *)((int)param_2 + 0x6a) = (short)param_1[5];
  *(undefined2 *)(param_2 + 0x1b) = *(undefined2 *)((int)param_1 + 0x16);
  *(short *)(param_2 + 0x1c) = (short)param_1[4];
  *(short *)((int)param_2 + 0x72) = (short)param_1[4];
  *(short *)(param_2 + 0x1d) = (short)param_1[4];
  TMD_OBJ_1784();
  return;
}
