/* FUN_80106134 @ 0x80106134  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106134(void)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 == 2) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
        uVar3 = func_0x8001af20();
        if ((uVar3 & 7) == 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 8;
        }
        iVar4 = func_0x8001a780(&DAT_800aca54);
        if (iVar4 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        }
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      }
      goto LAB_80106358;
    }
    if (bVar1 != 0) goto LAB_80106358;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar3 = func_0x8001af20();
    *(undefined *)(_DAT_800ac784 + 0x94) = (&DAT_8011dfe4)[uVar3 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar2 = *(char *)(_DAT_800ac784 + 6);
    if (cVar2 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_8011dfec);
      cVar2 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar2 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar3 = func_0x8001af20();
    if ((uVar3 & 7) == 0) {
      func_0x800453d0(6);
    }
    (*(code *)(&PTR_LAB_8011df80)[*(byte *)(_DAT_800ac784 + 5)])();
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
LAB_80106358:
  FUN_80109488(0,0);
  func_0x800245d8(0x800);
  return;
}


