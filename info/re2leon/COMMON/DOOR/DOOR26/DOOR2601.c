int DOOR2601(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -12281, -10226, -9211, 0, 8, 0);
	Obj_model_move(1, 0, 0, 1, 3, 53248, 4096, 18686, 16116, -32761, 0, 8, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	goto Sub17();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(12, 0x1806);
	Sleep(12800);
	Evt_exec(13, 0x180B);
	Sleep(14080);
	goto Sub12();
	return 0;
}