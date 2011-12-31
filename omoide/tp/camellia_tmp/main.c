/* main.c
 * Copyright (C) 2008 梅どぶろく umedoblock
 */

#include "../utbi_h\\camellia\\camellia.h"
#include <stdio.h>
#include <time.h>
unt fsize(wchar_t *fname);
unt fwrt(wchar_t *fname);

int main(void)
{
	SHA512 keyhash, mH[1], bH[1];
	uchar buf[32*CM_BLOCKSIZE], m[1024], c[1024];
	CAMELLIA_KEY cmk, cmen, cmde;
	clock_t start, end;
	double entm, detm;
	int i, ch, j;
	unt mBytes, cBytes, mtmp, ctmp;
	FILE *fm, *fc, *fb, *ft;

	for(i=0;i<32*CM_BLOCKSIZE;i++){
		buf[i] = 'F' + (uchar)i;
	}

	cmk.keysize = 256;
	sha512_Data(&keyhash, buf, 32*CM_BLOCKSIZE);
	camellia_hashset(&cmk, &keyhash);
	camellia_destroy(&cmk);

	sha512_clear(&keyhash);

	cmk.keysize = 256;
	sha512_Data(&keyhash, buf, 32*CM_BLOCKSIZE);
	camellia_hashset(&cmk, &keyhash);
	camellia_keygen(&cmk);
	camellia_keycopy(&cmen, &cmk);
	camellia_keycopy(&cmde, &cmk);
	camellia_destroy(&cmk);

	/**********************************************************************/
	start = clock();
	mBytes = fsize(L"m.txt");
	end = clock();
	mtmp = mBytes = 100*1024*1024;
	/**********************************************************************/
	printf("m.txt は %uBytes\n", mBytes);
	printf("読み込み速度 :%lfMBytes\n", (double)(mBytes/(1024.0*1024.0))/((double)(end-start)/CLOCKS_PER_SEC));
	printf("%lf秒\n", (double)(end-start)/CLOCKS_PER_SEC);
	fm = _wfopen(L"m.txt", L"rb");
	fc = _wfopen(L"c.txt", L"wb");

	/**********************************************************************/
	start = clock();

	cBytes = camellia_cBytes(mBytes);
	ctmp = cBytes;

	fwrite(&cBytes, sizeof(unt), 1, fc);
	
	mBytes = 0;
	camellia_encrypt_cbc_FileFile(fc, fm, mBytes, &cmen);
	mBytes = mtmp;
	fclose(fm);
	fclose(fc);
	end = clock();
	/**********************************************************************/

	entm = (double)(end-start)/CLOCKS_PER_SEC;
	printf("暗号化時間: %lf秒\n", entm);
	printf("暗号化速度: %lfMB/s\n", (mtmp/(1024*1024))/entm);

	cBytes = 0;

	fc = _wfopen(L"c.txt", L"rb");
	fb = _wfopen(L"b.txt", L"wb");

	/**********************************************************************/
	start = clock();
	fread(&cBytes, sizeof(unt), 1, fc);
	cBytes = ctmp;
	camellia_decrypt_cbc_FileFile(fb, fc, cBytes, &cmde);

	fclose(fc);
	fclose(fb);
	end = clock();
	/**********************************************************************/
	detm = (double)(end-start)/CLOCKS_PER_SEC;

	printf("暗号化時間: %lf秒\n", entm);
	printf("　復号時間: %lf秒\n", detm);
	printf("暗号化速度: %lfMB/s\n", (mtmp/(1024*1024))/entm);
	printf("　復号速度: %lfMB/s\n", (ctmp/(1024*1024))/detm);

	printf("ハッシュ値算出中\n");
	sha512_FileName(mH, L"m.txt");
	printf("m.txt終わり\n");
	sha512_FileName(bH, L"b.txt");
	printf("c.txt終わり\n");

	if(sha512_equal(mH[0], bH[0])){
		printf("camellia_CBC is correct\n");
		putchar('\a');putchar('\a');putchar('\a');putchar('\a');
	}else{
		printf("。・゜・(ノД`)・゜・。 うえええん\n");
		putchar('\a');
	}
	sha512_clear(mH);
	sha512_clear(bH);

	fm = _wfopen(L"m.txt", L"rb");
	ft = _wfopen(L"t.txt", L"wb");
	j = 160;
	for(i=0;i<j;i++){
		ch = fgetc(fm);
		fputc(ch, ft);
	}
	fclose(fm);
	fclose(ft);

	ft = _wfopen(L"t.txt", L"rb");
	fb = _wfopen(L"b.txt", L"wb");
	cBytes = camellia_cBytes(j);
	camellia_encrypt_cbc_FileData(c, ft, j, &cmen);
	camellia_decrypt_cbc_DataFile(fb, c, cBytes, &cmde);
	fclose(ft);
	fclose(fb);
	printf("ハッシュ値算出中\n");
	sha512_FileName(mH, L"t.txt");
	printf("m.txt終わり\n");
	sha512_FileName(bH, L"b.txt");
	printf("b.txt終わり\n");

	if(sha512_equal(mH[0], bH[0])){
		printf("camellia_CBC is correct\n");
		putchar('\a');putchar('\a');putchar('\a');putchar('\a');
	}else{
		printf("。・゜・(ノД`)・゜・。 うえええん\n");
		putchar('\a');
	}

	camellia_destroy(&cmde);
	camellia_destroy(&cmen);
	return 0;
}

unt fsize(wchar_t *fname)
{
	FILE *fin;
	uchar buf[2048];
	unt n, nByte=0;

	fin = _wfopen(fname, L"rb");

	while(fread(buf, sizeof(uchar), 2048, fin)==2048){
	}
	fclose(fin);
	nByte += n;

	return 0;
}
unt fwrt(wchar_t *fname)
{
	FILE *fout;
	uchar buf[2048];
	unt n, nByte=2048;

	fout = _wfopen(fname, L"wb");

	for(n=0;n<1024*1024*100/2048;n++){
		fwrite(buf, sizeof(uchar), nByte, fout);
	}

	fclose(fout);

	return 1;
}


