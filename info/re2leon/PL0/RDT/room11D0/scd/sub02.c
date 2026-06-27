int sub02(void) {

	Work_set(1, 0);
	nop();
	if(Cmp(26 = 1024)) {
		if(Member_cmp(9 = 512)) {
			if(Ck(5, 0, 0)) {
				Set(5, 0, 1);
				Set(5, 1, 0);
			}
		}
		if(Member_cmp(9 = 768)) {
			if(Ck(5, 1, 0)) {
				Set(5, 1, 1);
				Set(5, 0, 0);
				goto Sub05();
			}
		}
	}
	Sleep(256);
	goto(0x0000AEFF);
	return 0;
}