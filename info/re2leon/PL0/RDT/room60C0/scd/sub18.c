int sub18(void) {

goto Sub21();
Evt_next();
nop();
Xa_on(0, 2048);
Sleep(512);
Message_on(0, 4864, 0x0000);
Work_set(1, 0);
nop();
Plc_neck(5, 3, 0, 0, 0, 8224);
Sleep(10240);
Plc_motion(0, 17, 8);
Sleep(2560);
Plc_motion(0, 19, 0);
Plc_cnt(5);
Sleep(7168);
Plc_motion(0, 15, 0);
Plc_neck(2, 0, 0, 4, 0, 8224);
Sleep(2560);
Plc_neck(2, 0, 0, 252, 0, 8224);
Sleep(4608);
Plc_neck(2, 0, 0, 4, 0, 16448);
Sleep(1792);
Plc_neck(2, 0, 0, 252, 0, 16448);
Sleep(1280);
Plc_motion(0, 22, 0);
Sleep(512);
Plc_neck(5, 3, 0, 0, 0, 8224);
Wsleep();
Wsleeping();
return 0;
}