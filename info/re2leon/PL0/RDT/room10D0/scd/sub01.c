int sub01(void) {

	if(Ck(1, 0, 0)) {
		Ck(1, 1, 1)
		Ck(8, 106, 1)
		Ck(4, 80, 0)
		if(Ck(6, 196, 1)) {
			Ck(5, 10, 0)
			if(Ck(6, 199, 0)) {
				Evt_exec(255, 0x1802);
				Set(6, 199, 1);
			}
			Sce_bgm_control(0, 5, 0, 0, 0);
			Set(5, 10, 1);
		}
	}
	return 0;
}