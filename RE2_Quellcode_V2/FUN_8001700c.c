/* FUN_8001700c @ 0x8001700c  (Ghidra headless, raw MIPS overlay) */

void FUN_8001700c(int param_1,uint param_2)

{
  uint *puVar1;
  char cVar2;
  
  cVar2 = '\t';
  puVar1 = (uint *)(*(int *)(param_1 + 0x18) + 0x44);
  do {
    cVar2 = cVar2 + -1;
    puVar1[-10] = puVar1[-10] & 0xff000000 | param_2;
    *puVar1 = *puVar1 & 0xff000000 | param_2;
    puVar1 = puVar1 + 0x1a;
  } while (cVar2 != '\0');
  return;
}


