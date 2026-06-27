int DOOR0104(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -10226, -783, 0, 8, 0);
	Obj_model_move(2, 0, 0, 1, 1, 53248, 4096, 32511, -12044, -15117, 8, 8, 0);
	Obj_model_move(3, 0, 0, 1, 0, 32770, 4096, -12281, -10226, 1038, 0, 0, 0);
	Obj_model_move(4, 0, 0, 1, 1, 54016, 4096, -32256, -12044, -15117, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	goto Sub14();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1805);
	Sleep(12800);
	Evt_exec(13, 0x180B);
	Sleep(28160);
	Evt_exec(11, 0x1807);
	Evt_exec(12, 0x1809);
	Sleep(12800);
	return 0;
}