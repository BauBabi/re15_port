/* FUN_8010383c @ 0x8010383c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010383c(undefined4 param_1,undefined4 param_2,int param_3)

{
  char cVar1;
  int extraout_v1;
  uint *puVar2;
  
  puVar2 = (uint *)((uint)(byte)(&LAB_8011f784)[*(byte *)(param_3 + 8)] * 0xac +
                   *(int *)(param_3 + 0x188));
  func_0x80019700(0x2000,(int)*(short *)(param_3 + 0x6a),puVar2 + 0x10,&stack0x00000010);
  puVar2[0x1a] = 0x6f;
  *(undefined2 *)(puVar2 + 0x25) = 0;
  *(undefined2 *)((int)puVar2 + 0x96) = 0;
  *(undefined2 *)(puVar2 + 0x26) = 0;
  *(undefined2 *)((int)puVar2 + 0x9a) = 0;
  *(undefined2 *)(puVar2 + 0x27) = 0;
  *(undefined2 *)((int)puVar2 + 0x9e) = 0;
  *puVar2 = *puVar2 | 0x4a;
  func_0x800453d0(9);
  *(undefined1 *)(*(int *)(extraout_v1 + -0x387c) + 0x8f) = 7;
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),0x20
                 );
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80104a38();
  return;
}


