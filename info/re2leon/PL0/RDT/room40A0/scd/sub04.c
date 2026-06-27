int sub04(void) {

	Work_set(2, 0);
	Plc_motion(2, 14, 4);
	nop();
	Sleep(512);
	while(Ck(5, 11, 0)) {
		Sleep(2048);
		Se_on(2, 8, 1, 2, 0, 0, 0, 0);
		Sce_espr_on(0x00, 0x18, 0, 160, 11, 0x800E, 11265, 0, -27136, 8);
		Sce_espr_on(0x00, 0x03, 8, 160, 11, 0x800E, 11265, 0, -27136, 8);
		Sleep(512);
		Sce_espr_on(0x00, 0x1A, 5, 0, 0, 0x8003, -13296, -30470, -3905, 0);
		Sleep(256);
		Sce_espr_on(0x00, 0x03, 8, 160, 11, 0x09, 11265, 0, 0, 8);
		Sce_espr_on(0x00, 0x03, 8, 160, 11, 0x09, 11265, 0, 0, 8);
		Sce_espr_on(0x00, 0x02, 0, 160, 11, 0x07, 0, 0, 0, 8);
		Sleep(1792);
	}
	Plc_neck(2, 0, 0, 0, 14591, 5140);
	Sleep(2048);
	Se_on(2, 8, 1, 2, 0, 0, 0, 0);
	Sce_espr_on(0x00, 0x18, 0, 160, 11, 0x800E, 11265, 0, 12800, 8);
	Sce_espr_on(0x00, 0x03, 8, 160, 11, 0x800E, 11265, 0, 12800, 8);
	Sleep(768);
	Sce_espr_on(0x00, 0x03, 8, 160, 11, 0x09, 11265, 0, 0, 8);
	Sce_espr_on(0x00, 0x03, 8, 160, 11, 0x09, 11265, 0, 0, 8);
	Sce_espr_on(0x00, 0x02, 0, 160, 11, 0x07, 0, 0, 0, 8);
	Sleep(1792);
	Sleep(512);
	Plc_neck(5, 1, 0, 0, 0, 10280);
	Plc_motion(2, 9, 144);
	Sleep(7680);
	Plc_motion(2, 2, 4);
	while(Ck(5, 2, 0)) {
		Evt_next();
		nop();
	}
	Sce_espr_kill(3, 8, 160, 11);
	Sce_espr_kill(2, 0, 160, 11);
	Sce_espr_kill(24, 0, 160, 11);
	nop();
	Sleep(2048);
	Plc_motion(0, 15, 0);
	return 0;
}