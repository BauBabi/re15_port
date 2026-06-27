int sub14(void) {

	Work_set(3, 0);
	nop();
	Pos_set(0, 23194, 0, -23651);
	Dir_set(0, 0, 13580, 0);
	Plc_neck(5, 1, 0, 0, 0, 24672);
	Plc_motion(0, 15, 0);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 1056, -2916, 28353);
	Plc_neck(2, 0, 0, 16385, 16385, 8224);
	Work_set(4, 0);
	nop();
	Super_set(0, 64, 129, -24320, 9473, 23809, 4, 16629, 0);
	Sleep(2560);
	Xa_on(0, 5632);
	Message_on(0, 2304, 0x0000);
	Sleep(512);
	Set(1, 27, 1);
	Sleep(5120);
	Work_set(1, 0);
	nop();
	Plc_dest(0, 2336, -7015, 7840);
	Plc_neck(5, 3, 0, 0, 0, 24672);
	Wsleep();
	Wsleeping();
	Cut_chg(0x00);
	Sleep(2560);
	Sce_bgm_control(2, 1, 0, 0, 0);
	Sleep(2560);
	Work_set(3, 0);
	Plc_motion(0, 21, 0);
	nop();
	Sleep(8960);
	return 0;
}