int DOOR1801(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -10226, 8, 0, 0, 0);
	Obj_model_move(1, 0, 226, 1, 1, 53248, 4096, -32256, 31987, -16142, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	goto Sub08();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1803);
	Sleep(12800);
	Evt_exec(11, 0x1805);
	Sleep(15360);
	goto Sub06();
	return 0;
}