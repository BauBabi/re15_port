int sub12(void) {

	Sce_espr_kill(13, 0, 0, 0);
	Sce_espr_kill(14, 0, 0, 0);
	Sce_espr_kill(4, 3, 0, 0);
	nop();
	Sleep(10240);
	Evt_next();
	nop();
	Sce_espr_on(0x00, 0x04, 3, 0, 0, 0x24, 30377, -25345, 31420, 6656);
	return 0;
}