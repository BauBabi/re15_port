int sub01(void) {

	if(Ck(4, 152, 0)) {
		Cmp(26 = 1024)
		Set(5, 0, 1);
		Evt_exec(255, 0x1805);
	}
	if(Ck(5, 0, 1)) {
		Ck(5, 1, 0)
		if(Cmp(26 = 256)) {
			Set(5, 1, 1);
			Cut_replace(4, 2);
			Cut_be_set(2, 1, 1);
			Cut_be_set(2, 3, 0);
			nop();
		}
		if(Cmp(26 = 768)) {
			Set(5, 1, 1);
			Cut_replace(4, 2);
			Cut_be_set(2, 1, 1);
			Cut_be_set(2, 3, 0);
			nop();
		}
	}
	if(Ck(1, 11, 1)) {
		goto Sub02();
	}
	return 0;
}