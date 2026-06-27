int sub25(void) {

	Set(4, 164, 1);
	Se_on(2, 16, 1, 1, 0, 0, 0, 0);
	Set(1, 12, 1);
	Evt_exec(255, 0x181C);
	if(Ck(1, 1, 0)) {
		Set(29, 14, 1);
	}
	Aot_reset(29, 4, 49, 2304, 0, 0xEF);
	Sce_espr_kill(22, 16, 0, 0);
	nop();
	Sce_espr3d_on(0, 0x16, 0x00, 0, 0, 0x0A, -25370, 31481, 206, 0, 4, 0, 0);
	return 0;
}