int DOOR2402(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -10226, -16135, 0, 8, 0);
	Obj_model_move(1, 0, 228, 1, 1, 20484, 4096, -32256, -18190, -8462, -28667, 0, 0);
	Obj_model_move(2, 0, 228, 1, 1, 53252, 4096, 32511, -18190, -8462, -28659, 8, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	goto Sub08();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1803);
	Evt_exec(12, 0x1804);
	Sleep(12800);
	Evt_exec(11, 0x1805);
	Sleep(26880);
	goto Sub07();
	return 0;
}