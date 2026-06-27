/* FUN_80026ed8 @ 0x80026ed8  (Ghidra headless, raw MIPS overlay) */

void FUN_80026ed8(void)

{
  bool bVar1;
  int iVar2;
  byte bVar3;
  int iVar4;
  
  iVar4 = *(int *)(DAT_800ce324 + 0x2c) + DAT_800d4820 * 0x28;
  FUN_80076a00(&DAT_800df350,iVar4,0x28);
  *(undefined1 *)(iVar4 + 3) = 0;
  *(undefined2 *)(iVar4 + 0x26) = 1;
  DAT_1f80002c = 0x4b0;
  DAT_1f800030 = 0;
  FUN_80077384((int)DAT_800cfc6e,&DAT_1f80002c,&DAT_1f80002c);
  DAT_800df37c = 0xfffff6a0;
  DAT_800df34b = 0xff;
  DAT_800df34c = 0xe6;
  DAT_800df34d = 0xa0;
  DAT_800df378 = DAT_800cfc30 + DAT_1f80002c;
  DAT_800df380 = DAT_800cfc38 + DAT_1f800030;
  if (DAT_800df34a == '\0') {
    if (*(char *)(iVar4 + 10) != -1) {
      *(undefined1 *)(iVar4 + 10) = 0xff;
    }
    if (*(byte *)(iVar4 + 0xb) < DAT_800df34c) {
      *(byte *)(iVar4 + 0xb) = DAT_800df34c;
    }
    bVar1 = *(byte *)(iVar4 + 0xc) < DAT_800df34d;
    bVar3 = DAT_800df34d;
  }
  else {
    if (DAT_800df34a != '\x01') goto LAB_800270a0;
    if (*(byte *)(iVar4 + 10) < 0x7f) {
      *(undefined1 *)(iVar4 + 10) = 0x7f;
    }
    if (*(byte *)(iVar4 + 0xb) < DAT_800df34c >> 1) {
      *(byte *)(iVar4 + 0xb) = DAT_800df34c >> 1;
    }
    bVar1 = *(byte *)(iVar4 + 0xc) < DAT_800df34d >> 1;
    bVar3 = DAT_800df34d >> 1;
  }
  if (bVar1) {
    *(byte *)(iVar4 + 0xc) = bVar3;
  }
  DAT_800df34a = DAT_800df34a + '\x01';
LAB_800270a0:
  *(short *)(iVar4 + 0x1c) = (short)DAT_800df378;
  *(short *)(iVar4 + 0x1e) = (short)DAT_800df37c;
  iVar2 = DAT_800df380;
  *(undefined2 *)(iVar4 + 0x26) = 6000;
  *(short *)(iVar4 + 0x20) = (short)iVar2;
  return;
}


