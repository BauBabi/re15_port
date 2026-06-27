int sub13(void) {

Cut_chg(0x06);
Sleep(512);
Xa_on(0, 768);
Sleep(512);
Message_on(0, 3584, 0x0000);
Work_set(3, 0);
nop();
Plc_neck(2, 0, 0, 0, 4, 8224);
Plc_motion(0, 21, 0);
Sleep(10752);
Plc_motion(0, 17, 0);
Sleep(5120);
Plc_neck(5, 1, 0, 0, 0, 8224);
Plc_neck(2, 0, 0, -32768, 4, 2056);
Sleep(6400);
Plc_neck(5, 1, 0, 0, 0, 2056);
Sleep(6400);
Plc_motion(0, 17, 144);
Sleep(10240);
Plc_motion(0, 16, 0);
Plc_neck(2, 0, 0, 0, 4, 2056);
Sleep(5120);
Plc_motion(0, 19, 0);
Plc_cnt(5);
Sleep(1280);
Plc_neck(2, 0, 0, 0, -32513, 8224);
Wsleep();
Wsleeping();
return 0;
}