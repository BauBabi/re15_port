/* FUN_8011723c @ 0x8011723c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011723c(void)

{
  byte *pbVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  short *unaff_s0;
  
  if (DAT_800acae7 == 0) {
    unaff_s0 = (short *)&DAT_800aca88;
    iVar4 = func_0x8001a804(3000,0x180,&DAT_800aca88);
    if ((iVar4 < 0) && (*(short *)(_DAT_800ac784 + 0x1dc) == 0)) {
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      uVar3 = func_0x8001a9cc(&DAT_800aca88,0x20);
      *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar3;
      if ((*(short *)(_DAT_800ac784 + 0x1da) != 0) && (*(short *)(_DAT_800ac784 + 0x1da) < 1)) {
        return;
      }
      uVar5 = func_0x8001a804(0x9c4,0x100,&DAT_800aca88);
      if ((uVar5 & 0x80000000) == 0) {
        return;
      }
      if (*(short *)(_DAT_800ac784 + 0x1d6) != 0) {
        return;
      }
      if (*(char *)(_DAT_800ac784 + 0x1e1) != '\0') {
        return;
      }
      *(undefined1 *)(_DAT_800ac784 + 5) = 0xf;
      FUN_80117c78();
      return;
    }
  }
  iVar4 = _DAT_800ac784;
  if (DAT_800acae7 != 0) {
    if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 3000) &&
        ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) &&
       (pbVar1 = (byte *)(_DAT_800ac784 + 7), *pbVar1 != 0)) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 0;
      *unaff_s0 = *pbVar1 - 6;
      *(undefined2 *)(iVar4 + 0x1dc) = 0x2d;
      func_0x800453d0();
      DAT_800aca58 = 2;
      cVar2 = func_0x8001a780(unaff_s0 + -0x4d);
      DAT_800aca59 = cVar2 + '\x02';
      DAT_800aca5a = 0;
      if (*unaff_s0 < 0) {
        DAT_800aca58 = 3;
        DAT_800aca59 = '\0';
      }
      DAT_800acae7 = DAT_800acae7 | 1;
      FUN_80118524();
      return;
    }
    if (DAT_800acae7 != 0) {
      return;
    }
  }
  if (((*(ushort *)(_DAT_800ac784 + 0x1d4) < 0x1771) &&
      ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) && (*(char *)(_DAT_800ac784 + 7) != '\0')) {
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  if (((DAT_800acae7 == 0) && (6000 < *(ushort *)(_DAT_800ac784 + 0x1d4))) &&
     ((*(ushort *)(_DAT_800ac784 + 0x1d0) & 1) != 0)) {
    *(undefined1 *)(_DAT_800ac784 + 5) = 4;
    *(undefined1 *)(_DAT_800ac784 + 6) = 0;
    *(undefined1 *)(_DAT_800ac784 + 7) = 0;
  }
  return;
}


