/* FUN_80119198 @ 0x80119198  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80119198(void)

{
  int iVar1;
  int iVar2;
  byte bVar3;
  ushort uVar4;
  undefined2 uVar5;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  uint local_1c;
  
  bVar3 = *(byte *)(_DAT_800ac784 + 0x93);
  if ((bVar3 & 2) != 0) {
    if ((bVar3 & 0x40) != 0) {
      *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
      *(undefined1 *)(_DAT_800ac784 + 4) = 2;
      *(undefined1 *)(_DAT_800ac784 + 5) = 9;
      iVar1 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 6) = 3;
      iVar2 = _DAT_800ac784;
      *(undefined1 *)(_DAT_800ac784 + 7) = 0;
      *(short *)(iVar1 + 0x1d8) = (short)iVar2;
      if ((*(ushort *)(_DAT_800ac784 + 0x1d8) - 5 < 8) ||
         (*(ushort *)(_DAT_800ac784 + 0x1d8) - 0x21 < 5)) {
        uVar4 = func_0x8001af20();
        *(ushort *)(_DAT_800ac784 + 0x8c) = (uVar4 & 0x3f) + 0x50;
        bVar3 = func_0x8001af20();
        *(byte *)(_DAT_800ac784 + 0x9e) = (bVar3 & 0x30) + 0x30;
        if (*(ushort *)(_DAT_800ac784 + 0x1d4) < 2000) {
          *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + '\x18';
        }
        func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
      }
      func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0
                      ,0x200);
      local_1c = (uint)(*(byte *)(_DAT_800ac784 + 0x95) < 0x19);
      if (local_1c == 0) {
        uVar5 = func_0x8001bff8(0x39c,0);
        *(undefined2 *)(_DAT_800ac784 + 0x1d8) = uVar5;
        if ((DAT_800acae7 == '\0') && (*(short *)(_DAT_800ac784 + 0x1d8) != 0)) {
          *(undefined1 *)(_DAT_800ac784 + 6) = 2;
        }
        else {
          if (*(char *)(_DAT_800ac784 + 0x95) != '\x0f') {
            return;
          }
          *(undefined1 *)(_DAT_800ac784 + 6) = 7;
          *(undefined1 *)(_DAT_800ac784 + 0x95) = 0x23;
        }
        *(undefined4 *)(_DAT_800ac784 + 0x78) =
             *(undefined4 *)((uint)*(byte *)(_DAT_800ac784 + 0x1e2) * 4 + -0x7fedec98);
        func_0x800453d0();
        (**(code **)((uint)*(byte *)(_DAT_800ac784 + 5) * 4 + -0x7fedeb00))();
        return;
      }
      FUN_8011c024(0,1);
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = bVar3 & 0xfd;
    local_28 = _DAT_801213a8;
    local_24 = _DAT_801213ac;
    local_20 = _DAT_801213b0;
    local_1c = _DAT_801213b4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&local_28);
  }
  return;
}


