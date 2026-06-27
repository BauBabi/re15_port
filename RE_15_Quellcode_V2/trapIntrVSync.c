void trapIntrVSync(void)

{
  code *pcVar1;
  undefined4 *puVar2;
  int iVar3;
  
  iVar3 = 0;
  puVar2 = &DAT_800787bc;
  DAT_800787dc = DAT_800787dc + 1;
  do {
    pcVar1 = (code *)*puVar2;
    puVar2 = puVar2 + 1;
    if (pcVar1 != (code *)0x0) {
      (*pcVar1)();
    }
    iVar3 = iVar3 + 1;
  } while (iVar3 < 8);
  return;
}
