int sub17(void) {

	Cut_chg(0x07);
	Work_set(3, 0);
	nop();
	Pos_set(0, 24220, 0, 16298);
	Sleep(256);
	Plc_dest(0, 1057, -8547, 3251);
	Xa_on(0, 3840);
	Message_on(0, 2816, 0x0000);
	Sleep(512);
	Sleep(1280);
	Work_set(1, 0);
	Plc_motion(0, 23, 0);
	nop();
	goto Sub28();
	Work_set(3, 0);
	Plc_motion(0, 16, 0);
	nop();
	Sleep(10240);
	Work_set(3, 0);
	Plc_motion(0, 15, 0);
	nop();
	Sleep(5120);
	Plc_motion(0, 16, 0);
	Plc_cnt(10);
	Sleep(5120);
	Plc_neck(2, 0, 0, 0, -32768, 8224);
	Sleep(1280);
	Plc_neck(2, 0, 0, 0, 0, 8224);
	Wsleep();
	Wsleeping();
	Sleep(2560);
	return 0;
}