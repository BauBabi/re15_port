int DOOR2C03(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32770, 4096, -12281, -10226, -9985, 0, 8, 0);
	Obj_model_move(1, 0, 0, 1, 0, 32770, 4096, -12281, -10226, 10240, 0, 0, 0);
	Obj_model_move(2, 0, 0, 1, 1, 53312, 4096, -18690, -29696, 1266, 0, 8, 0);
	Obj_model_move(3, 0, 0, 1, 1, 53568, 4096, -32256, -29696, 10240, 0, 0, 0);
	Obj_model_move(4, 0, 0, 1, 2, 53248, 4096, -18690, -4880, -23810, 0, 248, 0);
	Obj_model_move(5, 0, 0, 1, 2, 53504, 4096, -32256, -4880, -13570, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	goto Sub12();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1805);
	Work_set(5, 0);
	nop();
	Speed_set(0, 0);
	Speed_set(8, 768);
	Speed_set(2, 2048);
	goto Sub07();
	Speed_set(8, -513);
	goto Sub07();
	Evt_exec(11, 0x180A);
	Evt_exec(12, 0x1806);
	Sleep(28160);
	Sleep(12800);
	return 0;
}