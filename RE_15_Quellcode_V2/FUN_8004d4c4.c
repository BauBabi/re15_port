void FUN_8004d4c4(void)

{
  int iVar1;
  undefined *puVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = 10;
  iVar3 = 0xe60;
  do {
    puVar2 = &DAT_800b2b4c + iVar3;
    if ((&DAT_800b2b4d)[iVar3] != '\0') {
      while( true ) {
        do {
          iVar1 = (*(code *)(&PTR_LAB_800744a8)[**(byte **)((int)&DAT_800b2b68 + iVar3)])(puVar2);
        } while (iVar1 == 1);
        if ((iVar1 == 2) || ((char)puVar2[(char)(&DAT_800b2b4e)[iVar3] + 4] < '\0')) break;
        iVar1 = *(int *)((int)&DAT_800b2c8c + iVar3);
        *(int *)((int)&DAT_800b2c8c + iVar3) = iVar1 + -4;
        *(undefined4 *)((int)&DAT_800b2b68 + iVar3) = *(undefined4 *)(iVar1 + -4);
        puVar2[(char)(&DAT_800b2b4e)[iVar3] + 4] = puVar2[(char)(&DAT_800b2b4e)[iVar3] + 4] + -1;
      }
    }
    if (**(char **)((int)&DAT_800b2b68 + iVar3) == '\x01') {
      (&DAT_800b2b4d)[iVar3] = 0;
    }
    uVar4 = uVar4 + 1;
    iVar3 = iVar3 + 0x170;
  } while (uVar4 < 0xe);
  return;
}
