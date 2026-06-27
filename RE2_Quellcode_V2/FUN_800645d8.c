/* FUN_800645d8 @ 0x800645d8  (Ghidra headless, raw MIPS overlay) */

void FUN_800645d8(int param_1,undefined4 param_2,undefined4 param_3)

{
  undefined1 *puVar1;
  ushort uVar2;
  uint uVar3;
  char cVar4;
  int iVar5;
  byte bVar6;
  undefined4 uVar7;
  undefined1 auStack_10 [4];
  undefined2 local_c;
  
  puVar1 = auStack_10 + 3;
  uVar3 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar3) =
       *(uint *)(puVar1 + -uVar3) & -1 << (uVar3 + 1) * 8 | DAT_8001195c >> (3 - uVar3) * 8;
  auStack_10 = (undefined1  [4])DAT_8001195c;
  local_c = DAT_80011960;
  switch(*(undefined1 *)(param_1 + 6)) {
  case 0:
    uVar2 = *(ushort *)(auStack_10 + *(char *)(param_1 + 0x151) * 2);
    *(undefined1 *)(param_1 + 6) = 1;
    *(byte *)(param_1 + 0x1c0) = *(byte *)(param_1 + 0x1c0) & 0xed;
    *(uint *)(param_1 + 0x14c) = uVar2 + 0x70000;
    if ((*(ushort *)(param_1 + 0x21c) & 0x2000) != 0) {
      *(undefined4 *)(param_1 + 0x14c) = 0x70006;
      *(undefined1 *)(param_1 + 6) = 4;
      *(undefined4 *)(param_1 + 0x144) = 0;
      *(byte *)(param_1 + 0x1c0) = *(byte *)(param_1 + 0x1c0) | 0x12;
      if (*(short *)(param_1 + 0x10c) == 0) {
        return;
      }
      if (*(short *)(param_1 + 0x1c2) + -500 <= (int)*(short *)(param_1 + 0x10c)) {
        return;
      }
      uVar2 = *(ushort *)(auStack_10 + *(char *)(param_1 + 0x151) * 2);
      *(undefined1 *)(param_1 + 6) = 1;
      *(uint *)(param_1 + 0x14c) = uVar2 + 0x70000;
      return;
    }
  case 1:
    FUN_8002959c(param_1,*(undefined4 *)(param_1 + 0x180),*(undefined4 *)(param_1 + 0x184),0x200);
    break;
  case 2:
    iVar5 = FUN_80015614(param_1,DAT_800cfc30,DAT_800cfc38,1);
    *(undefined4 *)(param_1 + 0x14c) = 0x70009;
    if (iVar5 << 0x10 < 0) {
      *(undefined4 *)(param_1 + 0x14c) = 0x7000b;
    }
    *(undefined1 *)(param_1 + 6) = 3;
    *(undefined1 *)(param_1 + 0x16b) = 0;
  case 3:
    iVar5 = FUN_8002959c(param_1,*(undefined4 *)(param_1 + 0x180),*(undefined4 *)(param_1 + 0x184),
                         0x200);
    if ((iVar5 != 0) && (*(char *)(param_1 + 0x16b) == '\0')) {
      *(undefined1 *)(param_1 + 0x16b) = 1;
      *(char *)(param_1 + 0x14c) = *(char *)(param_1 + 0x14c) + '\x01';
    }
    break;
  case 4:
    uVar7 = 0x200;
    goto LAB_800647d4;
  case 5:
    bVar6 = *(byte *)(param_1 + 0x1c0) & 0xed;
    *(byte *)(param_1 + 0x1c0) = bVar6;
    if (6000 < *(ushort *)(param_1 + 0x21e)) {
      *(byte *)(param_1 + 0x1c0) = bVar6 | 0x12;
    }
    *(undefined4 *)(param_1 + 0x14c) = 0x70006;
    break;
  case 6:
    uVar7 = 0x10200;
LAB_800647d4:
    cVar4 = FUN_8002959c(param_1,param_2,param_3,uVar7);
    *(char *)(param_1 + 6) = *(char *)(param_1 + 6) + cVar4;
    break;
  case 7:
    *(undefined1 *)(param_1 + 6) = 0;
    *(ushort *)(param_1 + 0x21c) = *(ushort *)(param_1 + 0x21c) & 0xdfff;
  }
  return;
}


