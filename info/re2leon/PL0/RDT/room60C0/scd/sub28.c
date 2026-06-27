int sub28(void) {

Xa_on(0, 7168);
Message_on(0, 5632, 0x0000);
Work_set(1, 0);
Plc_motion(0, 24, 4);
nop();
Plc_neck(2, 0, 0, 255, 252, 8224);
Sleep(17920);
Plc_neck(2, 0, 0, 1, 252, 8224);
Sleep(12800);
Plc_neck(2, 0, 0, 255, 252, 8224);
Sleep(15360);
Plc_motion(0, 25, 0);
Sleep(3072);
Plc_neck(2, 0, 0, 1, 252, 8224);
Sleep(17920);
Plc_neck(2, 0, 0, 0, 0, 8224);
Sleep(12800);
Plc_neck(2, 0, 0, 1, 252, 8224);
Sleep(17920);
Plc_motion(0, 25, 128);
Plc_cnt(0);
Sleep(3072);
Plc_neck(6, 0, 0, 0, 0, 5140);
Wsleep();
Wsleeping();
return 0;
}