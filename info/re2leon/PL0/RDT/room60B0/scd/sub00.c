int sub00(void) {

	goto Sub06();
	if(Ck(3, 36, 1)) {
		Aot_reset(0, 0, 0, 0, 0, 0x00);
		Aot_reset(4, 1, 49, 44202, 0, 0x50AB);
	}
	Obj_model_set(0x00, 0, 0, 0, 0, 0, 0, 0, 2560, 0, 8270, 8270, 8270, 0, 242, 0, 0, 0, 0, 0, 0, 0);
	if(Ck(4, 106, 1)) {
		Aot_set_4p(5, 5, 49, 0, 0, -16190, 31915, 23751, -4945, 17611, 1196, -1850, 21671, 255, 0, 24);
		goto Sub04();
	} else {
		Aot_set_4p(5, 5, 49, 0, 0, -16190, 31915, 23751, -4945, 17611, 1196, -1850, 21671, 255, 0, 24);
		nop();
		nop();
		if(Ck(4, 95, 0)) {
			Cut_be_set(2, 2, 1);
			Cut_be_set(2, 3, 0);
			Cut_be_set(6, 1, 1);
			Cut_be_set(6, 2, 0);
			Cut_be_set(8, 1, 1);
			Cut_be_set(8, 3, 0);
			Sca_id_set(0, 0x10F8);
			Sca_id_set(1, 0x0004);
			Sca_id_set(2, 0x8000);
			Aot_set_4p(6, 4, 49, 0, 0, -16185, 5803, -12075, -31560, 18651, 17586, 14541, -26460, 3, 0, 0);
		} else {
			Cut_be_set(2, 2, 0);
			Cut_be_set(2, 3, 1);
			Cut_be_set(6, 1, 0);
			Cut_be_set(6, 2, 1);
			Cut_be_set(8, 1, 0);
			Cut_be_set(8, 3, 1);
			Work_set(4, 0);
			nop();
			Pos_set(0, -24375, -16150, -6998);
			Sca_id_set(0, 0x0000);
			Sca_id_set(1, 0x0000);
			Sca_id_set(2, 0x0000);
			Aot_reset(5, 4, 49, 512, 0, 0xFF);
			Set(21, 1, 0);
			Set(21, 3, 0);
			Evt_exec(255, 0x180C);
			nop();
			nop();
			goto Sub07();
			return 0;
}