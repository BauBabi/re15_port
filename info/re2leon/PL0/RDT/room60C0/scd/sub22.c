int sub22(void) {

Work_set(1, 0);
nop();
Pos_set(0, 1339, 0, 28343);
Set(2, 7, 1);
Set(1, 27, 1);
Evt_exec(255, 0x1817);
Sleep(2560);
goto Sub12();
goto Sub13();
goto Sub14();
goto Sub15();
goto Sub16();
goto Sub17();
goto Sub18();
goto Sub19();
// unknown opcode (8B) found at 0x00000032
// unknown opcode (8B) found at 0x00000038
// unknown opcode (8B) found at 0x0000003E
// unknown opcode (8B) found at 0x00000044
// unknown opcode (8B) found at 0x0000004A
// unknown opcode (8A) found at 0x00000050
// unknown opcode (8B) found at 0x00000056
// unknown opcode (8C) found at 0x0000005E
Set(1, 29, 1);
Sce_bgm_control(1, 2, 0, 0, 0);
Sce_bgm_control(2, 1, 0, 127, 64);
Se_on(2, 13, 1, 1, 0, 0, 0, 0);
Evt_exec(255, 0x181D);
goto Sub20();
Set(5, 1, 1);
Sleep(5120);
Sleep(2048);
Sleep(7680);
Sleep(7680);
Plc_neck(6, 0, 0, 0, 0, 12850);
Sleep(1536);
Evt_exec(255, 0x1819);
Evt_exec(255, 0x1818);
Evt_exec(255, 0x181A);
Cut_chg(0x06);
Sleep(7680);
Sleep(7680);
goto Sub21();
Set(1, 29, 0);
Se_on(2, 14, 1, 1, 0, 0, 0, 0);
Sleep(14080);
Work_set(1, 0);
nop();
Plc_dest(0, 1312, 10806, 5567);
goto Sub33();
Plc_motion(1, 6, 0);
Sleep(12800);
Plc_neck(2, 0, 0, 4, 1, 8224);
Sleep(2560);
Plc_neck(2, 0, 0, 252, 1, 8224);
Sleep(4608);
Plc_neck(2, 0, 0, 4, 1, 16448);
Sleep(1792);
Plc_neck(2, 0, 0, 252, 1, 16448);
Sleep(1280);
Plc_neck(6, 0, 0, 0, 0, 8224);
Sleep(10240);
Sleep(12800);
Plc_neck(2, 0, 0, -32768, 0, 8224);
Sleep(15360);
Aot_on(7);
while(Ck(1, 16, 1)) {
Evt_next();
nop();
}
Set(3, 161, 1);
Work_set(1, 0);
Plc_motion(1, 6, 192);
Plc_cnt(0);
nop();
Sleep(6144);
goto Sub28();
Sce_bgm_control(0, 1, 0, 127, 64);
goto Sub32();
Cut_auto(1);
Set(3, 36, 1);
Work_set(1, 0);
Plc_ret();
Set(2, 7, 0);
Set(1, 27, 0);
return 0;
}