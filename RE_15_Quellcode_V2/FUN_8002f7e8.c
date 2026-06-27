void FUN_8002f7e8(void)

{
  char cVar1;
  char *pcVar2;
  
  pcVar2 = &DAT_80073d14;
  cVar1 = *(char *)(DAT_800b52d8 + 6) + '\x0e';
  do {
    if (cVar1 == *pcVar2) {
      *pcVar2 = (&DAT_80073d08)[*(char *)(DAT_800b52d8 + 5)];
      break;
    }
    pcVar2 = pcVar2 + 1;
  } while (pcVar2 < "\x16\x16\x0f\x0f\x02\t\x0f\x0f");
  (&DAT_80073d08)[*(char *)(DAT_800b52d8 + 5)] = cVar1;
  return;
}
