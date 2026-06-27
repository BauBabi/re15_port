/* FUN_801067c8 @ 0x801067c8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801067c8(undefined4 param_1,undefined4 param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  
  if (*(char *)(_DAT_800ac784 + 7) != '\x01') {
    if (*(char *)(_DAT_800ac784 + 7) != '\0') {
      FUN_801072ec();
      return;
    }
    uVar3 = func_0x8001af20();
    if ((uVar3 & 1) == 0) {
      *(undefined1 *)(_DAT_800ac784 + 6) = 5;
      *(char *)(*(int *)(param_3 + -0x387c) + 7) =
           *(char *)(*(int *)(param_3 + -0x387c) + 7) + '\x05';
      FUN_80109488(0,1);
      FUN_80107d88();
      return;
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(char *)(_DAT_800ac784 + 0x94) = (*(char *)(_DAT_800ac784 + 0x93) >> 7) * '\x02' + '\r';
    bVar1 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar1 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011e2bc);
    puVar5 = (uint *)((uint)(byte)(&LAB_8011d8d4)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_8011e2bc);
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar5 + 0x10,&LAB_8011e2bc);
    puVar5[0x1a] = 0x8f;
    *(undefined2 *)((int)puVar5 + 0x96) = 0xffce;
    *(undefined2 *)(puVar5 + 0x25) = 0;
    *(undefined2 *)(puVar5 + 0x26) = 0;
    *(undefined2 *)((int)puVar5 + 0x9a) = 3;
    *(undefined2 *)(puVar5 + 0x27) = 0;
    *(undefined2 *)((int)puVar5 + 0x9e) = 0;
    *puVar5 = *puVar5 | 0x4a;
    func_0x800453d0(9);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    iVar4 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar4 + 0x548) = 0x80;
    *(undefined4 *)(iVar4 + 0x54c) = 0x20;
    *(uint *)(iVar4 + 0x4b4) = *(uint *)(iVar4 + 0x4b4) | 0x80;
    if (_DAT_800b0fe0 < 3) {
      FUN_80107258(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  FUN_801072ec();
  return;
}


