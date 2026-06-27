void FUN_8001f220(uint *param_1)

{
  short sVar1;
  
  sVar1 = *(short *)((int)param_1 + 0x9a);
  *(short *)((int)param_1 + 0x9a) = sVar1 + -1;
  if (sVar1 == 0) {
    *(undefined1 *)(param_1 + 0x26) = 0;
    *(undefined1 *)((int)param_1 + 0x99) = 0;
    if ((char)param_1[0x27] != '\0') {
      *(undefined1 *)((int)param_1 + 0x75) = 0x14;
      *(undefined2 *)((int)param_1 + 0x82) = 0;
      *(undefined1 *)((int)param_1 + 0x76) = 8;
      *(undefined2 *)(param_1 + 0xe) = 0;
      *(undefined2 *)((int)param_1 + 0x3a) = 0;
      *(undefined2 *)(param_1 + 0xf) = 0;
      *param_1 = *param_1 ^ 0x1062;
    }
  }
  return;
}
