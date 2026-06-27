int sub19(void) {

	Evt_next();
	Work_set(3, 0);
	Pos_set(0, 22169, 0, -29258);
	Dir_set(0, 0, 14575, 0);
	Se_on(2, 10, 0, 0, 0, 22169, 0, -29258);
	Sleep(5120);
	Work_set(1, 0);
	nop();
	Plc_neck(5, 3, 0, 0, 0, 24672);
	Sleep(1280);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 2336, 934, 9144);
	Sleep(1280);
	Se_on(2, 11, 0, 0, 0, 22169, 0, -29258);
	Cut_chg(0x02);
	Sleep(3840);
	Xa_on(0, 5632);
	Sleep(512);
	Message_on(0, 0, 0x0000);
	Work_set(3, 0);
	Plc_motion(0, 15, 0);
	nop();
	Sleep(3840);
	Plc_neck(2, 0, 0, 0, 4, 8224);
	Sce_bgm_control(0, 1, 0, 0, 0);
	Sleep(2560);
	Plc_neck(2, 0, 0, 0, 8192, 8224);
	Wsleep();
	Wsleeping();
	return 0;
}