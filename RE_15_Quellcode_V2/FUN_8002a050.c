void FUN_8002a050(undefined2 param_1,undefined2 param_2)

{
  char cVar1;
  char *pcVar2;
  char *pcVar3;
  char *pcVar4;
  u_long *puVar5;
  uint uVar6;
  
  uRam00000074 = 0x1e0;
  uRam00000076 = (undefined2)iRam0000000c;
  uRam00000070 = param_1;
  uRam00000072 = param_2;
  StoreImage((RECT *)0x70,puRam00000018);
  uVar6 = 0;
  if (iRam0000000c * 0x3c0 != 0) {
    pcVar3 = pcRam00000010 + 2;
    pcVar2 = (char *)((int)puRam00000018 + 1);
    pcVar4 = pcRam00000010;
    puVar5 = puRam00000018;
    do {
      if (((*pcVar4 != '\0') || (pcVar3[-1] != '\0')) || (*pcVar3 != '\0')) {
        cVar1 = *pcVar4;
        pcVar2[1] = cVar1;
        *pcVar2 = cVar1;
        *(char *)puVar5 = cVar1;
      }
      uVar6 = uVar6 + 3;
      pcVar2 = pcVar2 + 3;
      puVar5 = (u_long *)((int)puVar5 + 3);
      pcVar3 = pcVar3 + 3;
      pcVar4 = pcVar4 + 3;
    } while (uVar6 < (uint)(iRam0000000c * 0x3c0));
  }
  LoadImage((RECT *)0x70,puRam00000018);
  return;
}
