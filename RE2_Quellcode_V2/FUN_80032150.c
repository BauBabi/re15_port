/* FUN_80032150 @ 0x80032150  (Ghidra headless, raw MIPS overlay) */

void FUN_80032150(int param_1,undefined4 *param_2)

{
  ushort uVar1;
  int iVar2;
  
  iVar2 = param_1 * 0x20;
  uVar1 = *(ushort *)(&DAT_800a2180 + iVar2);
  if (uVar1 == 1) {
    DAT_800d763c = (undefined4 *)&DAT_8014fc20;
  }
  else if (uVar1 < 2) {
    DAT_800d763c = param_2;
    if (uVar1 == 0) {
      DAT_800d763c = &DAT_80198000;
    }
  }
  else {
    DAT_800d763c = DAT_800ce32c;
    if (uVar1 != 2) {
      DAT_800d763c = param_2;
    }
  }
  DAT_800d423a = (undefined1)param_1;
  DAT_800d4238 = *(short *)(&DAT_800a2178 + iVar2);
  DAT_800eae34 = DAT_800d763c + 2;
  DAT_800e8980 = DAT_800d763c + 8;
  DAT_800d7682 = *(undefined2 *)(&DAT_800a2176 + iVar2);
  DAT_800df338 = &DAT_800e2a88;
  DAT_800d7680 = 0;
  DAT_800d7684 = 0;
  DAT_800d75dc = 0x10;
  if (DAT_800d4238 == 1) {
    DAT_800d75dc = 0x18;
  }
  DAT_800d7624 = (undefined2)*(undefined4 *)(&DAT_800a217c + iVar2);
  DAT_800d7628 = *(undefined2 *)(&DAT_800a217e + iVar2);
  DAT_800d7632 = *(ushort *)(&DAT_800a2184 + iVar2);
  DAT_800d7634 = *(undefined2 *)(&DAT_800a2178 + iVar2);
  if ((DAT_800d7632 & 2) == 0) {
    DAT_800d7686 = 0;
  }
  else if (DAT_800d7686 == '\x02') {
    DAT_800d7686 = 1;
    DAT_800dfd5b = 2;
    DAT_800d7632 = DAT_800d7632 & 0xff7f | 0x40;
  }
  else {
    DAT_800d7686 = 0;
    DAT_800dfd5b = 0;
  }
  DAT_800d7688 = 0;
  DAT_800d7687 = 0;
  DAT_800d767e = (undefined2)(((uint)*(ushort *)(&DAT_800a2186 + iVar2) * (uint)DAT_800eae3c) / 100)
  ;
  DAT_800d7690 = *(undefined2 *)(&DAT_800a2174 + iVar2);
  DAT_800cfb74 = DAT_800cfb74 & 0xffff7fff | 0x200;
  DAT_800d762a = DAT_800d75dc;
  FUN_80039694();
  return;
}


