int DOOR1902(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -12281, -10226, -1800, 0, 8, 0);
	Obj_model_move(1, 0, 0, 1, 3, 53248, 4096, 22271, 4342, -2319, 0, 0, 8);
	Obj_model_move(2, 0, 0, 1, 2, 53504, 4096, -6656, 9730, -25086, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	goto Sub09();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1804);
	Sleep(12800);
	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(8, -769);
	Speed_set(2, -2305);
	goto Sub05();
	Speed_set(8, 1024);
	goto Sub05();
	Evt_exec(11, 0x1807);
	Sleep(28160);
	return 0;
}