int DOOR2A02(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -12281, -10226, 8, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	Sleep(12800);
	goto Sub10();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(8, -769);
	Speed_set(2, -2305);
	goto Sub06();
	Speed_set(8, 1024);
	goto Sub06();
	Evt_exec(11, 0x1808);
	Sleep(28160);
	return 0;
}