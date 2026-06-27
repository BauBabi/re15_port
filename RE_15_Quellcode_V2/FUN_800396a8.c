void FUN_800396a8(char param_1,undefined1 param_2)

{
  byte bVar1;
  undefined1 *puVar2;
  uint uVar3;
  
  bVar1 = *DAT_800ac778;
  uVar3 = 0;
  puVar2 = DAT_800b2584;
  if (bVar1 != 0) {
    do {
      uVar3 = uVar3 + 1;
      if (puVar2[1] == param_1) {
        *puVar2 = param_2;
      }
      puVar2 = puVar2 + 4;
    } while (uVar3 < bVar1);
  }
  return;
}
