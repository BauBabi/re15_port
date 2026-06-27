int DOOR0B02(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -10226, -16135, 0, 8, 0);
	Obj_model_move(1, 0, 189, 1, 1, 20484, 4096, -32256, -12044, -8973, 0, 0, 0);
	Obj_model_move(2, 0, 189, 1, 1, 53252, 4096, 32511, -18188, 27891, 8, 8, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	Sleep(12800);
	goto Sub08();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(11, 0x1805);
	Sleep(28160);
	goto Sub07();
	return 0;
}