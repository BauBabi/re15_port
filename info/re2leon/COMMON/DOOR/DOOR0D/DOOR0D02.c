int DOOR0D02(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -24562, -16135, 0, 8, 0);
	Obj_model_move(1, 0, 248, 1, 1, 20484, 4096, -32256, 30963, -32526, 0, 0, 0);
	Obj_model_move(2, 0, 248, 1, 1, 53252, 4096, 32511, 30963, -24334, 8, 8, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	goto Sub13();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(11, 0x1806);
	Evt_exec(12, 0x1807);
	Sleep(15360);
	Evt_exec(11, 0x180A);
	Sleep(28160);
	goto Sub12();
	return 0;
}