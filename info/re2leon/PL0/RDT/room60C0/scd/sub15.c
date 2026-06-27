int sub15(void) {

Cut_chg(0x06);
Evt_next();
nop();
Xa_on(0, 1280);
Sleep(512);
Message_on(0, 4096, 0x0000);
Work_set(3, 0);
Plc_motion(0, 16, 16);
nop();
Plc_neck(2, 0, 0, -32768, 4, 4100);
Sleep(5120);
Plc_neck(2, 0, 0, -32513, 4, 4100);
Sleep(5120);
Plc_neck(2, 0, 0, -32768, 4, 4100);
Sleep(5120);
Plc_neck(2, 0, 0, -32513, 4, 4100);
Sleep(4608);
Plc_motion(0, 26, 0);
Plc_cnt(5);
Sleep(10752);
Plc_motion(0, 16, 16);
Plc_cnt(5);
Plc_neck(5, 1, 0, 0, 0, 8224);
Sleep(12800);
Plc_motion(0, 17, 16);
Sleep(17920);
Plc_motion(0, 17, 128);
Sleep(2560);
Plc_motion(0, 17, 0);
Plc_cnt(20);
Sleep(7680);
Plc_motion(0, 16, 16);
Sleep(10240);
for(n = 0; n < 2560; n++) {
Plc_motion(0, 16, 0);
Plc_cnt(18);
Plc_neck(2, 0, 0, -32768, 4, 4104);
Evt_next();
nop();
Plc_neck(2, 0, 0, -32513, 4, 2056);
Evt_next();
nop();
}
Plc_motion(0, 16, 144);
Sleep(5120);
Plc_motion(0, 16, 8);
Plc_cnt(10);
Plc_neck(5, 1, 0, 0, 0, 16448);
Wsleep();
Wsleeping();
return 0;
}