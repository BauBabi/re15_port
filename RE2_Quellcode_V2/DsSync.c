/* DsSync @ 0x8008a8c4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

int DsSync(int id,u_char *result)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined4 *puVar5;
  
  if (id == 0) {
    iVar2 = (DAT_800c3f04 + -1) * 0x10;
    if (DAT_800c3f04 + -1 < 0) {
      iVar2 = 0x70;
    }
    puVar5 = (undefined4 *)0x0;
    if (*(int *)((int)&DAT_800c3e84 + iVar2) == 0) goto code_r0x8008aa8c;
  }
  else {
    DS_sync(0);
    iVar3 = 0;
    iVar2 = DAT_800c3f04 << 4;
    do {
      piVar1 = (int *)((int)&DAT_800c3e84 + iVar2);
      iVar2 = iVar2 + 0x10;
      if (*piVar1 == id) {
        iVar2 = DSSYS_2_OBJ_EA8();
        return iVar2;
      }
      if (0x7f < iVar2) {
        iVar2 = 0;
      }
      iVar3 = iVar3 + 1;
    } while (iVar3 < 8);
    if ((-(id < (int)(&DAT_800c3e84)[DAT_800c3f04 * 4]) & 7U) != 7) {
      return 0;
    }
    iVar3 = DAT_800c3f04 + -1;
    if (iVar3 < 0) {
      iVar3 = 7;
    }
    iVar4 = 0;
    if (id == 0) {
      iVar2 = iVar3 * 0x10;
      if ((&DAT_800c3e84)[iVar3 * 4] == 0) {
        iVar2 = DSSYS_2_OBJ_FD8();
        return iVar2;
      }
    }
    else {
      iVar2 = iVar3 << 4;
      while (iVar3 = iVar3 + -1, *(int *)((int)&DAT_800c3e84 + iVar2) != id) {
        iVar2 = iVar2 + -0x10;
        if (iVar3 < 0) {
          iVar2 = 0x70;
          iVar3 = 7;
        }
        iVar4 = iVar4 + 1;
        if (7 < iVar4) {
          iVar2 = DSSYS_2_OBJ_FD8();
          return iVar2;
        }
      }
    }
  }
  DAT_800c3d78 = *(undefined4 *)((int)&DAT_800c3e84 + iVar2);
  _DAT_800c3d7c = *(undefined4 *)(&DAT_800c3e88 + iVar2);
  DAT_800c3d80 = *(undefined4 *)(&DAT_800c3e8c + iVar2);
  DAT_800c3d84 = *(undefined4 *)(&UNK_800c3e90 + iVar2);
  puVar5 = &DAT_800c3d78;
code_r0x8008aa8c:
  if (puVar5 == (undefined4 *)0x0) {
    return 6;
  }
  rescpy(result,(int)puVar5 + 5);
  iVar2 = DSSYS_2_OBJ_FF8();
  return iVar2;
}


