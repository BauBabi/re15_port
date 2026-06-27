int sub12(void) {

Sce_bgm_control(1, 1, 0, 127, 64);
Xa_on(0, 256);
Work_set(3, 0);
Plc_motion(0, 16, 0);
nop();
Wsleep();
Wsleeping();
Cut_chg(0x09);
Evt_next();
nop();
Xa_on(0, 512);
Sleep(512);
Message_on(0, 3328, 0x0000);
Work_set(3, 0);
nop();
Plc_neck(2, 0, 0, -32513, 4, 2056);
Sleep(7680);
Plc_motion(0, 16, 144);
Sleep(5120);
Plc_neck(5, 1, 0, 0, 0, 15420);
Plc_motion(0, 16, 8);
Plc_cnt(10);
Sleep(10240);
Plc_motion(0, 19, 16);
Plc_cnt(5);
Plc_neck(2, 0, 0, -32768, 4, 8224);
Sleep(2048);
Plc_neck(2, 0, 0, -32513, 4, 8224);
Sleep(4096);
Plc_neck(2, 0, 0, -32768, 4, 8224);
Sleep(4096);
Plc_neck(2, 0, 0, -32513, 4, 8224);
Sleep(7680);
Plc_motion(0, 16, 8);
Plc_neck(5, 1, 0, 0, 0, 24672);
Sleep(7680);
Plc_neck(2, 0, 0, -26370, 4, 5140);
Sleep(5120);
Plc_neck(5, 1, 0, 0, 0, 5140);
Sleep(7680);
Plc_neck(2, 0, 0, 4, 0, 4128);
Sleep(6400);
Plc_neck(2, 0, 0, 252, 0, 4128);
Sleep(6400);
Plc_neck(2, 0, 0, 4, 0, 8256);
Sleep(1536);
Plc_neck(2, 0, 0, 252, 0, 8256);
Sleep(1536);
Plc_motion(0, 18, 0);
for(n = 0; n < 2560; n++) {
Plc_neck(2, 0, 0, -4865, 4, -14328);
Evt_next();
nop();
Plc_neck(2, 0, 0, 5120, 4, -14328);
Evt_next();
nop();
}
for(n = 0; n < 3840; n++) {
Plc_motion(0, 18, 0);
Plc_cnt(18);
Plc_neck(2, 0, 0, -32768, 4, 4104);
Evt_next();
nop();
Plc_neck(2, 0, 0, -32513, 4, 2056);
Evt_next();
nop();
}
Plc_motion(0, 23, 0);
Plc_neck(5, 1, 0, 0, 0, -14136);
Wsleep();
Wsleeping();
return 0;
}