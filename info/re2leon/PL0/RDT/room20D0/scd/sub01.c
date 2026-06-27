int sub01(void) {

	if(Cmp(2 = 12032)) {
		Evt_exec(255, 0x1805);
		Se_on(2, 15, 0, 0, 0, 0, 0, 0);
	}
	if(Ck(1, 1, 1)) {
		Ck(4, 79, 0)
		Ck(4, 78, 1)
		if(Ck(6, 198, 1)) {
			Ck(5, 20, 0)
			if(Ck(6, 201, 0)) {
				Evt_exec(255, 0x1802);
				Set(6, 201, 1);
			}
			Sce_bgm_control(0, 5, 0, 0, 0);
			Set(5, 20, 1);
		}
	}
	return 0;
}