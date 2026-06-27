/* FUN_8011970c @ 0x8011970c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011970c(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  char in_v0;
  char cVar2;
  undefined2 uVar3;
  ushort uVar4;
  int iVar5;
  undefined1 *puVar6;
  byte unaff_s1;
  int unaff_s2;
  char unaff_s3;
  char unaff_s4;
  
  *(char *)(*(int *)(param_3 + -0x387c) + 6) = *(char *)(*(int *)(param_3 + -0x387c) + 6) + in_v0;
  iVar5 = FUN_8011c024(0,0);
  do {
    if (*(short *)(iVar5 + 0x1da) != 0) {
      unaff_s4 = '\x01';
    }
    do {
      bVar1 = unaff_s1 == 0;
      unaff_s1 = unaff_s1 - 1;
      if (bVar1) {
        if ((unaff_s3 == '\0') && (unaff_s4 == '\0')) {
          return;
        }
        _DAT_800acaee = _DAT_800acaee - 0xc;
        *(undefined2 *)(_DAT_800ac784 + 0x1dc) = 0x2d;
        func_0x800453d0(6);
        DAT_800aca58 = 2;
        puVar6 = &DAT_800aca54;
        cVar2 = func_0x8001a780();
        DAT_800aca59 = cVar2 + '\x02';
        DAT_800aca5a = 0;
        if ((short)_DAT_800acaee < 0) {
          DAT_800aca58 = 3;
          DAT_800aca59 = '\0';
        }
        uVar4 = (ushort)DAT_800acae7;
        DAT_800acae7 = (byte)(uVar4 | 1);
        if (_DAT_800acaee != (uVar4 | 1)) {
          FUN_8011bb8c();
          return;
        }
        puVar6[0x93] = 0;
        *(undefined1 *)(_DAT_800ac784 + 4) = 1;
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        *(undefined1 *)(_DAT_800ac784 + 6) = 0;
        *(undefined1 *)(_DAT_800ac784 + 7) = 0;
        if (*(char *)(_DAT_800ac784 + 0x1e3) != '\0') {
          *(undefined1 *)(_DAT_800ac784 + 5) = 9;
        }
        return;
      }
    } while (*(char *)(_DAT_800ac784 + 0x95) != *(char *)(unaff_s1 + 0x80121488));
    uVar3 = func_0x8001bff8(unaff_s2 + 0x6f8,&stack0x00000010,800,&DAT_800aca88);
    *(undefined2 *)(_DAT_800ac784 + 0x1da) = uVar3;
    iVar5 = _DAT_800ac784;
  } while( true );
}


