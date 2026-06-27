int sub00(void) {

	Sce_em_set(0, 0, 0x1F, 2, 64, 0, 0, 3, -1, -31198, 0, -22619, 28682, 1, 1024);
	Work_set(3, 0);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 || 512)
	Member_set2(0, 16);
	nop();
	if(Ck(4, 152, 0)) {
		goto Sub03();
		Cut_be_set(2, 1, 0);
		Cut_be_set(2, 3, 1);
		Cut_replace(2, 4);
		nop();
	} else {
		goto Sub04();
		Cut_be_set(2, 1, 1);
		Cut_be_set(2, 3, 0);
		Evt_exec(255, 0x1807);
		nop();
		nop();
		Evt_exec(255, 0x1808);
		goto Sub02();
		return 0;
}