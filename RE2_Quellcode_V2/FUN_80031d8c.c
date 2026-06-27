/* FUN_80031d8c @ 0x80031d8c  (Ghidra headless, raw MIPS overlay) */

void FUN_80031d8c(void)

{
  ushort uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  ushort *puVar4;
  
  puVar4 = &DAT_800d76a4;
  puVar3 = (undefined4 *)&DAT_800d76ac;
  do {
    uVar1 = *puVar4;
    DAT_800d7824 = puVar4;
    if (uVar1 == 2) {
      FUN_800957a4();
      uVar2 = OpenTh(puVar3[-1],puVar3[2],puVar3[0xf]);
      *puVar3 = uVar2;
      FUN_800957b4();
      *puVar4 = 0x7f;
LAB_80031e40:
      ChangeTh(*puVar3);
    }
    else if (uVar1 < 3) {
      if ((uVar1 == 1) &&
         (uVar1 = *(short *)((int)puVar3 + -6) - 1, *(ushort *)((int)puVar3 + -6) = uVar1,
         (int)((uint)uVar1 << 0x10) < 1)) {
LAB_80031e3c:
        *puVar4 = 0x7f;
        goto LAB_80031e40;
      }
    }
    else if (uVar1 == 4) goto LAB_80031e3c;
    puVar4 = puVar4 + 0x40;
    puVar3 = puVar3 + 0x20;
    if (&UNK_800d7823 < puVar4) {
      return;
    }
  } while( true );
}


