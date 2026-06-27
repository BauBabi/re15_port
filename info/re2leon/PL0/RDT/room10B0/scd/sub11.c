int sub11(void) {

	Set(2, 7, 1);
	Set(2, 2, 1);
	Set(2, 3, 1);
	Cut_chg(0x08);
	Sce_Item_lost(51);
	Aot_reset(15, 0, 65, 0, 0, 0x00);
	Work_set(4, 1);
	nop();
	Pos_set(0, 31154, -23306, 30361);
	Sleep(768);
	Se_on(2, 11, 0, 0, 0, -30542, -29194, 30361);
	Sleep(15360);
	Cut_chg(0x03);
	Cut_auto(1);
	if(Ck(4, 18, 1)) {
		if(Ck(4, 19, 1)) {
			if(Ck(4, 43, 0)) {
				Set(4, 43, 1);
				Evt_exec(255, 0x180C);
			}
		}
	}
	Set(2, 3, 0);
	Set(2, 2, 0);
	Set(2, 7, 0);
	return 0;
}