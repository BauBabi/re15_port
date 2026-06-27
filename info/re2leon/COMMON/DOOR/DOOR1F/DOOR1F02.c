int DOOR1F02(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -12281, 10481, 4341, 0, 0, 0);
	Obj_model_move(1, 0, 226, 1, 1, 36864, 4096, 0, -21991, 0, 0, 0, 0);
	Obj_model_move(2, 0, 0, 1, 2, 53248, 4096, 0, 19980, 28690, 0, 8, 0);
	Obj_model_move(3, 0, 0, 1, 3, 53248, 4096, 0, -5621, -28671, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(10240);
	Evt_exec(11, 0x1803);
	Evt_exec(12, 0x1804);
	Sleep(5120);
	goto Sub05();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Sleep(61440);
	return 0;
}