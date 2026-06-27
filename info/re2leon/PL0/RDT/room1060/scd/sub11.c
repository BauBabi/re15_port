int sub11(void) {

	Set(2, 7, 1);
	Set(4, 1, 1);
	Aot_reset(9, 4, 49, 1536, 0, 0xFFFF);
	Set(5, 1, 1);
	Sca_id_set(6, 0x0000);
	Sce_fade_set(0, 2, 7, 0, 4);
	Sleep(8192);
	Work_set(4, 0);
	nop();
	Pos_set(0, 13996, 9730, -29955);
	Member_set(15, 8);
	Work_set(1, 0);
	nop();
	Pos_set(0, 125, 0, 12405);
	Cut_chg(0x04);
	Sce_fade_set(0, 2, 7, 0, 252);
	Sleep(5120);
	Evt_exec(255, 0x180D);
	Evt_exec(255, 0x180E);
	Sleep(1280);
	Se_on(2, 12, 1, 4, 0, 0, 0, 0);
	Sleep(5120);
	Evt_exec(255, 0x180C);
	Sleep(7680);
	Evt_exec(255, 0x1806);
	Sleep(9728);
	Evt_exec(255, 0x180F);
	Sleep(24320);
	Work_set(4, 0);
	nop();
	Pos_set(0, 125, 0, 125);
	Work_set(1, 0);
	nop();
	Pos_set(0, 32425, 0, 6397);
	Work_set(3, 0);
	nop();
	Pos_set(0, -21092, 0, 4859);
	Member_copy(16, 0);
	nop();
	Calc(16 && 63487)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 && 65471)
	Member_set2(7, 16);
	nop();
	Member_set(2, 256);
	Member_set(3, 0);
	Member_set(4, 0);
	Member_set(5, 0);
	Work_set(3, 1);
	nop();
	Pos_set(0, 5784, 0, -13569);
	Member_copy(16, 0);
	nop();
	Calc(16 && 63487)
	Member_set2(0, 16);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 && 65471)
	Member_set2(7, 16);
	nop();
	Member_set(2, 256);
	Member_set(3, 0);
	Member_set(4, 0);
	Member_set(5, 0);
	Cut_chg(0x00);
	Cut_auto(1);
	Aot_reset(0, 4, 49, 1280, 0, 0xFFFF);
	Sca_id_set(6, 0x00FC);
	Set(2, 7, 0);
	return 0;
}