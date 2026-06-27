int sub12(void) {

	Set(2, 7, 1);
	Set(2, 2, 1);
	Set(2, 3, 1);
	Cut_replace(10, 11);
	Cut_replace(2, 12);
	Cut_replace(3, 13);
	Cut_replace(4, 14);
	Cut_replace(5, 15);
	Cut_chg(0x0D);
	Evt_next();
	if(Ck(1, 1, 0)) {
		Sce_bgm_control(0, 2, 0, 0, 0);
		Movie_on(4);
		Evt_next();
		Sce_bgm_control(0, 1, 0, 0, 0);
		nop();
	} else {
		if(Ck(5, 3, 0)) {
			if(Ck(4, 171, 0)) {
				Movie_on(4);
				Evt_next();
				nop();
			} else {
				if(Ck(5, 2, 0)) {
					Sce_bgm_control(0, 2, 0, 0, 0);
					Sce_bgm_control(1, 2, 0, 0, 0);
					Movie_on(4);
					Evt_next();
					Sce_bgm_control(0, 1, 0, 0, 0);
					Sce_bgm_control(1, 1, 0, 0, 0);
					nop();
				} else {
					Sce_bgm_control(0, 2, 0, 0, 0);
					Movie_on(4);
					Evt_next();
					Sce_bgm_control(0, 1, 0, 0, 0);
					nop();
					nop();
					nop();
					nop();
					nop();
				} else {
					Sce_bgm_control(0, 2, 0, 0, 0);
					Movie_on(4);
					Evt_next();
					Sce_bgm_control(0, 1, 0, 0, 0);
					nop();
					nop();
					nop();
					nop();
					nop();
					Work_set(4, 2);
					nop();
					if(Ck(1, 0, 0)) {
						Pos_set(0, -29525, 12788, 24987);
						Dir_set(0, -32765, 176, 0);
					} else {
						Pos_set(0, -13397, 2548, -16230);
						Dir_set(0, 16639, 0, 0);
						nop();
						nop();
						Cut_auto(1);
						Set(2, 3, 0);
						Set(2, 2, 0);
						Set(2, 7, 0);
						return 0;
}