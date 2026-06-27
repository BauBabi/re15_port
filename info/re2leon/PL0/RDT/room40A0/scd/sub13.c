int sub13(void) {

	Cut_replace(6, 13);
	Cut_replace(7, 14);
	Sca_id_set(0, 0x00D0);
	Set(29, 16, 1);
	Sleep(43520);
	for(n = 0; n < 1536; n++) {
		goto Sub14();
	}
	return 0;
}