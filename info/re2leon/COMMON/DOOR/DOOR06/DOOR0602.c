int DOOR0602(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -24562, -16135, 0, 8, 0);
	Obj_model_move(1, 0, 248, 1, 1, 20484, 4096, -32256, 16628, 10995, 0, 0, 0);
	Obj_model_move(2, 0, 248, 1, 1, 53252, 4096, 32511, 8948, -3854, 8, 8, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	Sleep(15360);
	goto Sub12();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(11, 0x1809);
	Sleep(28160);
	goto Sub11();
	return 0;
}