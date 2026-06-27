int sub01(void) {

	if(Ck(4, 69, 0)) {
		if(Ck(1, 1, 1)) {
			if(Ck(8, 110, 1)) {
				Evt_exec(255, 0x1802);
				Set(4, 69, 1);
			}
		} else {
			if(Ck(8, 56, 1)) {
				Aot_reset(2, 5, 65, 65280, 6146, 0x00);
				Set(4, 69, 1);
			}
			nop();
			nop();
		}
		if(Ck(6, 108, 1)) {
			Ck(5, 20, 0)
			Sce_bgm_control(1, 5, 0, 0, 0);
			Set(5, 20, 1);
		}
		return 0;
}