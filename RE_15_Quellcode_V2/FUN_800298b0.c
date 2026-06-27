void FUN_800298b0(void)

{
  ushort uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  ushort *puVar4;
  
  puVar4 = &DAT_800b2924;
  puVar3 = (undefined4 *)&DAT_800b292c;
  do {
    uVar1 = *puVar4;
    DAT_800b2b24 = puVar4;
    if (uVar1 == 2) {
      FUN_8006e170();
      uVar2 = OpenTh(puVar3[-1],puVar3[2],puVar3[0xf]);
      *puVar3 = uVar2;
      FUN_8006e468();
      *puVar4 = 0x7f;
LAB_80029964:
      ChangeTh(*puVar3);
    }
    else if (uVar1 < 3) {
      if ((uVar1 == 1) &&
         (uVar1 = *(short *)((int)puVar3 + -6) - 1, *(ushort *)((int)puVar3 + -6) = uVar1,
         (int)((uint)uVar1 << 0x10) < 1)) {
LAB_80029960:
        *puVar4 = 0x7f;
        goto LAB_80029964;
      }
    }
    else if (uVar1 == 4) goto LAB_80029960;
    puVar4 = puVar4 + 0x40;
    puVar3 = puVar3 + 0x20;
    if (&UNK_800b2aa3 < puVar4) {
      return;
    }
  } while( true );
}
