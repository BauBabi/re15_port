int sub13(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Work_set(4, 8);
	nop();
	Pos_set(0, 20390, -31240, 17343);
	Member_set(15, 25612);
	Work_set(1, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 2048)
	Member_set2(0, 16);
	nop();
	if(Ck(1, 0, 0)) {
		Pos_set(0, 4512, 27648, 13501);
	} else {
		Pos_set(0, 3489, 27648, 25277);
		nop();
		nop();
		Member_set(15, 12);
		Evt_next();
		nop();
		if(Ck(4, 74, 0)) {
			Set(4, 74, 1);
			Evt_exec(255, 0x180E);
			Evt_exec(8, 0x180F);
		} else {
			Set(4, 74, 0);
			Evt_exec(255, 0x1810);
			Evt_exec(8, 0x1811);
			nop();
			nop();
			Sleep(35328);
			Evt_kill(9);
			Evt_kill(8);
			Sce_fade_set(2, 2, 7, 0, 24);
			Sleep(512);
			Work_set(1, 0);
			nop();
			Pos_set(0, -5210, 0, 28860);
			Member_set(15, 12);
			Member_copy(16, 0);
			nop();
			Calc(16 && 63487)
			Member_set2(0, 16);
			nop();
			Plc_ret();
			nop();
			Sce_fade_set(2, 2, 7, 0, 232);
			Work_set(4, 8);
			nop();
			Pos_set(0, 20390, 26026, 17343);
			Member_set(15, 25612);
			Cut_auto(1);
			Sce_Item_lost(67);
			Set(1, 16, 1);
			Set(1, 27, 0);
			Set(2, 7, 0);
			return 0;
}