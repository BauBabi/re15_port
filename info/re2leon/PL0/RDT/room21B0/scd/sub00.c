int sub00(void) {

	goto Sub02();
	Item_aot_set(5, 2, 49, 0, 0, 20141, -12125, -30716, -23546, 0x1400, 3840, 512, 0xFF00);
	Sca_id_set(12, 0x0000);
	Work_set(4, 0);
	nop();
	Pos_set(0, 1196, 19711, -4945);
	Member_set(15, 16385);
	if(Ck(4, 62, 0)) {
		Set(20, 1, 0);
	} else {
		Work_set(4, 2);
		nop();
		Member_set(13, 31911);
		Work_set(4, 3);
		nop();
		Member_set(12, 13812);
		Work_set(4, 4);
		nop();
		Member_set(12, 31221);
		Work_set(4, 5);
		nop();
		Member_set(12, -19210);
		Work_set(4, 1);
		nop();
		Member_set(12, -10070);
		Sca_id_set(13, 0x0000);
		Set(16, 4, 0);
		nop();
		nop();
		if(Cmp(26 = 3840)) {
			if(Cmp(27 = 6914)) {
				goto Sub03();
			} else {
				goto Sub04();
				nop();
				nop();
			}
			return 0;
}