/*
	pcdrvとcdromの使用例
	original asm:k-comm.
	C           :bero
*/

// cdrom: and pcdrv: stuff

#include	"syscall.h"

int main(int argc,char **argv)
{
	struct DIRENTRY	*dir,dirbuf[80];
	int	fsize,rsize,wsize;
	int fd;
	static char cdfile[]="cdrom:SYSTEM.CNF;1";
	static char Filename[]="pcdrv:test.txt";
	char buf[2048*2];


	ExitCriticalSection();				//割り込み許可(for CD-ROM)
	//dir = firstfile(cdfile, dirbuf);	//ファイル存在チェック

	// change PC's directory to djgpp
	//cd("pcdrv:/djgpp");
	// change cdrom dir to S2
	cd("cdrom:/S2");
	// try different devices here
	dir = firstfile("cdrom:", dirbuf);
	while(dir != NULL) {
		printf("%4c    %8d  %s\n", dir->system, dir->size, dir->name);
		dir = nextfile(dir);
	}
	if (dir==NULL) goto ERR;			//存在しなければエラー
	fsize = dir->size;
	
	//fd = open(cdfile,O_RDONLY);		//リードオープン
	//if (fd<0) goto ERR;
	//rsize = read(fd,buf,(fsize+2047)&~2047); //2048バイト単位に補正
	//close(fd);
	//if (rsize<0) goto ERR;
	//dir = firstfile(Filename,dirbuf);	//ファイル存在チェック
	//if (dir!=NULL) {					//存在する場合は削除
	//	if (!delete(Filename))
	//		goto ERR;
	//}
	//fd = open(Filename,O_CREAT);		//新規作成
	//if (fd<0) goto ERR;
	//wsize = write(fd,buf,fsize);
	//if (wsize<0) {
	//	close(fd);
	//	goto ERR;
	//}
	//close(fd);
	puts("OK");							//正常終了
	return 0;
ERR:
	puts("Error!");						//異常終了
	return -1;
}

