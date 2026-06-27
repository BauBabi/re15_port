int sub19(void) {

	Cut_chg(0x07);
	Evt_next();
	nop();
	Xa_on(0, 4352);
	Message_on(0, 3328, 0x0000);
	Sleep(512);
	Work_set(3, 0);
	Plc_motion(0, 18, 0);
	nop();
	Sleep(1280);
	Evt_exec(255, 0x1814);
	Sleep(1280);
	Plc_neck(4, 2, 0, 0, 0, 20560);
	Sleep(6400);
	Plc_neck(2, 0, 0, 0, 1, 4112);
	Sleep(5120);
	return 0;
}