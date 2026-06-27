int DOOR0101(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -24562, 8, 0, 0, 0);
	if(Cmp(14 = 0)) {
		Obj_model_move(1, 0, 0, 1, 1, 53248, 4096, -32256, 28915, -23310, 0, 0, 0);
	} else {
		Obj_model_move(1, 0, 0, 1, 3, 53248, 4096, -32256, 28915, -23310, 0, 0, 0);
		nop();
		nop();
		Sce_fade_set(0, 2, 7, 0, 254);
		Sce_fade_adjust(0, 0, 28);
		Sleep(17920);
		goto Sub14();
		Se_on(0, 0, 0, 0, 0, 0, 0, 0);
		Evt_exec(11, 0x1805);
		Evt_exec(12, 0x1806);
		Sleep(12800);
		Evt_exec(13, 0x180B);
		Sleep(15360);
		goto Sub12();
		return 0;
}