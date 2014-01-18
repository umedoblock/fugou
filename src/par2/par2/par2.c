/* par2.c
 * Copyright (C) 2008 梅濁酒 umedoblock
 */

#include "par2.h"
extern unsigned char par2_add_8[256][256];
extern unsigned char par2_mul_8[256][256];
extern unsigned char par2_div_8[256][256];
extern unsigned char par2_f_8[256][256];

int par2_inverse(unsigned char **inv, unsigned char **x, int n_parity);
int par2_mult(
	unsigned char *data, unsigned char **inv,
	unsigned char *parity, int n_parity);

int fgetline(char *ss, size_t buflen, FILE *fp)
{
	int len;
	fgets(ss, buflen, fp);
	len = (int )strlen(ss);
	while((len > 0) && (isspace(ss[len-1]) != 0)){
		ss[len-1] = '\0';
		len--;
	}
	return 0;
}

void dump(unsigned char *buf, int buflen, int width)
{
	int i;
	for(i=0;i<buflen;i++){
		fprintf(stdout, "%02x, ", buf[i]);
		if((i+1)%width==0){
			fprintf(stdout, "\n");
		}
	}
}

void bitset(unsigned char *bit, int no)
{
	int y, x;
	x = 7 - (no % 8);
	y = no / 8;
	bit[y] |= (1 << x);
}

void bitclear(unsigned char *bit, int no)
{
	int y, x;
	x = 7 - (no % 8);
	y = no / 8;
	bit[y] &= ~(1 << x);
}

int bitcheck(unsigned char *bit, int no)
{
	int y, x;
	x = 7 - (no % 8);
	y = no / 8;
	return (bit[y] & (1 << x)) != 0;
}

unsigned char par2_encode_octet(
	unsigned char *data, int n_parity, int parityindex)
{
	int i;
	unsigned char tmp0, tmp1, tmp2, v = 0;

	for(i=0;i<n_parity;i++){
		tmp0 = data[i];
		tmp1 = par2_f_8[parityindex][i];
		tmp2 = par2_mul_8[tmp0][tmp1];
		v = par2_add_8[v][tmp2];
	}

	return v;
}

int par2_encode_block(
	unsigned char *parity, unsigned char *data, int n_parity)
{
	int i;
	for(i=0;i<n_parity;i++){
		parity[i] = par2_encode_octet(data, n_parity, i);
	}
	return n_parity;
}

//parityとdataはmemory領域が重なってはならない。
//parityはdata以上の長さのmemory領域でなければならない。
int par2_encode_data(
	unsigned char *parity, unsigned char *data,
	int datalen, int n_parity
)
{
	int i, offset = 0, cnt;
	cnt = datalen / n_parity;

	for(i=0,offset=0;i<cnt;i++,offset+=n_parity){
		par2_encode_block(parity+offset, data+offset, n_parity);
	}

	//成功時にはparity長を返す。
	return datalen;
}

int par2_tateyoko(
	unsigned char *yx, unsigned char *xy,
	int n_parity, int paritylen)
{
	int i, j;

	for(j=0;j<n_parity;j++){
		for(i=0;i<paritylen;i++){
			yx[j*paritylen+i] = xy[i*n_parity+j];
		}
	}

	return 0;
}

int par2_yokotate(
	unsigned char *xy, unsigned char *yx,
	int n_parity, int paritylen)
{
	int i, j;

	for(j=0;j<n_parity;j++){
		for(i=0;i<paritylen;i++){
			xy[i*n_parity+j] = yx[j*paritylen+i];
		}
	}

	return 0;
}

int par2_bufwrite(
	FILE **fo, int n_parity, int buflen,
	unsigned char *wbuf, unsigned char *data
)
{
	int i, woffset = buflen / n_parity;

	par2_tateyoko(wbuf, data, n_parity, buflen / n_parity);
	for(i=0;i<n_parity;i++){
		fwrite(wbuf+woffset*i, 1, woffset, fo[i]);
	}
	return 0;
}

int par2_encode_file(char *filepath, char *filename, int n_parity)
{
	size_t len_path = strlen(filepath), len_name = strlen(filename);
	size_t len = len_path + len_name, high = 0, low = 0;
	char *path = (char *)malloc(len + 16 + 1);
	FILE *fi, **fo, *fp;
	unsigned char *parity, *data, *wbuf;
	int i, datalen, woffset, amari, padlen, run = 1;
	int buflen = BUFFER_SIZE - (BUFFER_SIZE % n_parity);

	sprintf(path, "%s/%s", filepath, filename);
	fi = fopen(path, "rb");
	sprintf(path, "%s/%s.parity", filepath, filename);
	fp = fopen(path, "wb");
	fprintf(fp, "%s\n", filename);
	fprintf(fp, "0x%08x\n", n_parity);

	data = (unsigned char *)malloc(buflen * 3);
	parity = data + buflen;
	wbuf = parity + buflen;

	fo = (FILE **)malloc(sizeof(FILE *) * 2*n_parity);
	for(i=0;i<2*n_parity;i++){
		sprintf(path, "%s/%s.pari.%d", filepath, filename, i);
		fo[i] = fopen(path, "wb");
		fprintf(fp, "%s.pari.%d\n", filename, i);
	}
	woffset = buflen / n_parity;
	while(run){
		datalen = fread(data, 1, buflen, fi);
		low += (size_t )datalen;
		if(low < (size_t )datalen){
			high++;
		}
		if(datalen != buflen){
			if(datalen == 0)break;
			amari = datalen % n_parity;
			if(amari != 0){
				padlen = n_parity - amari;
				memset(data + datalen, 0, padlen);
				datalen += padlen;
				run = 0;
			}
		}
		par2_encode_data(parity, data, datalen, n_parity);

		par2_bufwrite(fo, n_parity, datalen, wbuf, data);
		par2_bufwrite(fo + n_parity, n_parity, datalen, wbuf, parity);
	}
	fprintf(fp, "0x%08x",   high);
	fprintf(fp,   "%08x\n", low);

	for(i=2*n_parity-1;i>=0;i--){
		fclose(fo[i]);
	}
	free(fo);
	free(data);
	fclose(fp);
	fclose(fi);
	free(path);

	return 0;
}

typedef struct {
	unsigned char **pinv;
	unsigned char **px;
	unsigned char *inv;
	unsigned char *x;
	unsigned char *xtmp;
} tagpar2_decode;

tagpar2_decode *par2_decode_block_malloc(int n_parity)
{
	int i, n2_parity = n_parity * n_parity;
	tagpar2_decode *dec;

	dec  = (tagpar2_decode *)malloc(sizeof(tagpar2_decode));
	dec->inv  = (unsigned char *)malloc(n2_parity);
	dec->x    = (unsigned char *)malloc(n2_parity);
	dec->xtmp = (unsigned char *)malloc(n2_parity);
	dec->pinv = (unsigned char **)malloc(sizeof(unsigned char *)*n_parity);
	dec->px   = (unsigned char **)malloc(sizeof(unsigned char *)*n_parity);

	for(i=0;i<n_parity;i++){
		dec->pinv[i] = dec->inv + n_parity * i;
		dec->px[i]   = dec->x   + n_parity * i;
	}

	return dec;
}

int par2_decode_block_free(tagpar2_decode *dec)
{
	free(dec->px);
	free(dec->pinv);
	free(dec->xtmp);
	free(dec->x);
	free(dec->inv);
	free(dec);
	return 0;
}

int par2_decode_parity(
	unsigned char *flg, tagpar2_decode *dec, int n_parity
)
{
	int i, j;

	for(i=0,j=0;i<n_parity;i++){
		if(bitcheck(flg, i)){
			memset(dec->xtmp+i*n_parity, 0, n_parity);
			dec->xtmp[i*n_parity+i] = 1;
		}else{
			while(bitcheck(flg, n_parity + j) == 0)j++;
			memcpy(dec->xtmp+i*n_parity, par2_f_8[j], n_parity);
			j++;
		}
	}

	memcpy(dec->x, dec->xtmp, n_parity * n_parity);
	par2_inverse(dec->pinv, dec->px, n_parity);

	return 0;
}

int par2_decode_block(
	unsigned char *data, unsigned char *parity,
	int n_parity, tagpar2_decode *dec)
{
	par2_mult(data, dec->pinv, parity, n_parity);

	return 0;
}

int par2_argcheck(int n_parity, unsigned char *flg)
{
	int i, count;

	if(n_parity <= 1 || n_parity > 256){
		return -2;
	}
	//n_parityとして有効な値は2〜256

	if(n_parity == 256){
		//分割数として256が選択できないのは現在の制限事項。
		//分割数として128, 189, 215などは指定できる。
		//256のみが分割数として指定できない。
		return -3;
	}

	for(i=0,count=0;i<2*n_parity;i++){
		count += bitcheck(flg, i);
	}
	if(count < n_parity){
		return -4;
	}

	return 0;
}

//parityとdataはmemory領域が重なってはならない。
//dataはparity以上の長さのmemory領域でなければならない。
int par2_decode_data(
	unsigned char *data, unsigned char *parity,
	int datalen, int n_parity, unsigned char *flg
)
{
	int i, offset = 0, cnt, ret;
	tagpar2_decode *dec;
	cnt = datalen / n_parity;

	ret = par2_argcheck(n_parity, flg);
	if(ret < 0){
		return ret;
	}
	dec = par2_decode_block_malloc(n_parity);
	par2_decode_parity(flg, dec, n_parity);
	for(i=0,offset=0;i<cnt;i++,offset+=n_parity){
		par2_decode_block(data+offset, parity+offset, n_parity, dec);
	}
	par2_decode_block_free(dec);

	//成功時にはparity長を返す。
	return datalen;
}

int count_data_file(
	FILE *fi[], unsigned char *flg, int n_parity,
	FILE *fparity, char *path, char *filepath, int count)
{
	int i;
	char ss[FILENAME_SIZE+1];

	for(i=0;i<n_parity;i++){
		fgetline(ss, sizeof(ss), fparity);
		sprintf(path, "%s/%s", filepath, ss);
		fi[i] = fopen(path, "rb");
		if(fi[i] != NULL){
			fprintf(stdout, "_data_file fi[%d]->fp = %s\n", i, path);
			bitset(flg, i);
			count++;
			fprintf(stdout, "count = %d\n", count);
		}
	}

	return count;
}

int count_parity_file(
	FILE *fi[], unsigned char *flg, int n_parity,
	FILE *fparity, char *path, char *filepath, int count)
{
	int i, j;
	char ss[FILENAME_SIZE+1];
	FILE *fp;
	for(j=n_parity;j<2*n_parity;j++){
		if(count >= n_parity){
			break;
		}
		fgetline(ss, sizeof(ss), fparity);
		sprintf(path, "%s/%s", filepath, ss);
		fp = fopen(path, "rb");
		if(fp != NULL){
			for(i=0;i<n_parity;i++){
				if(fi[i] == NULL){
					fprintf(stdout, "_parity_file fi[%d]->fp = %s\n", i, path);
					fi[i] = fp;
					bitset(flg, j);
					count++;
					fprintf(stdout, "count = %d\n", count);
					break;
				}
			}
		}
	}
	for(;j<2*n_parity;j++){
		fgetline(ss, sizeof(ss), fparity);
	}

	return count;
}

int par2_decode_file_free(
	FILE *fi[], int n_parity, FILE *fo)
{
	int i;
	for(i=n_parity-1;i>=0;i--){
		if(fi[i] != NULL){
			fclose(fi[i]);
		}
	}
	fclose(fo);

	return 0;
}

int fread_filesize_64bit(FILE *fp, size_t *high, size_t *low)
{
	char ss[FILENAME_SIZE+1];
	int tmp;

	fgetline(ss, sizeof(ss), fp);
	tmp = ss[10];
	ss[10] = '\0';
	sscanf(ss, "%x", high);
	ss[10] = tmp;
	strncpy(ss+8, "0x", 2);
	sscanf( ss+8, "%x", low);

	return 0;
}

int par2_decode_file_init(
	tagpar2_decode_file *p2df,
	char *filepath, char *filename)
{
	int ret = 0, amari, i, count = 0;
	char ss[FILENAME_SIZE+1], name[PATH_SIZE+1];
	char path[FILENAME_SIZE+PATH_SIZE+1+16+1];

	p2df->fo = p2df->fparity = NULL;

	if(	strlen(filepath) > PATH_SIZE ||
		strlen(filename) > FILENAME_SIZE){
		return PAR2_ERROR_PATH_LENGTH;
	}
	sprintf(path, "%s/%s", filepath, filename);
	fprintf(stdout, "p2df->fparity = %s\n", path);
	p2df->fparity = fopen(path, "r");

	if( p2df->fparity != NULL){
		fgetline(name, FILENAME_SIZE+1, p2df->fparity);
		sprintf(path, "%s/%s.tmp", filepath, name);
		fprintf(stdout, "p2df->fo = %s\n", path);
		p2df->fo = fopen(path, "wb");
	}

	if( p2df->fparity == NULL ||
		p2df->fo == NULL){
		fprintf(stdout, "par2_decode_file_init() error\n");
		if(p2df->fparity == NULL){
			fprintf(stdout, "p2df->fparity == NULL\n");
			ret = PAR2_ERROR_PARITY_FILE_GA_MITSUKARIMASEN;
		}else if(p2df->fo == NULL){
			fprintf(stdout, "p2df->fo == NULL\n");
			ret = PAR2_ERROR_KAKIKOMI_DEKIMASEN_DECODE_FILE;
		}
		if(p2df->fparity != NULL) fclose(p2df->fparity);
		if(p2df->fo != NULL) fclose(p2df->fo);
	}else{
		memset(p2df->flg, 0, PAR2_PARITY_FLG_SIZE);
		for(i=0;i<PAR2_MAX_N_PARITY;i++) p2df->fi[i] = NULL;

		fgetline(ss, sizeof(ss), p2df->fparity);
		sscanf(ss, "%x", &p2df->n_parity);

		amari = (BUFFER_SIZE % p2df->n_parity);
		p2df->buflen = BUFFER_SIZE - amari;

		count = count_data_file(p2df->fi, p2df->flg, p2df->n_parity, p2df->fparity, path, filepath, 0);
		count = count_parity_file(p2df->fi, p2df->flg, p2df->n_parity, p2df->fparity, path, filepath, count);
		if(count < p2df->n_parity){
			par2_decode_file_free(p2df->fi, p2df->n_parity, p2df->fo);
			fprintf(stdout, "必要なparityが集まりませんでした。\n");
			fprintf(stdout, "count = %d\n", count);
			ret = PAR2_ERROR_HITUYOUNA_PARITY_TARINAI;
		}

		fread_filesize_64bit(p2df->fparity, &p2df->high, &p2df->low);

		p2df->roffset = p2df->buflen / p2df->n_parity;
		fprintf(stdout, "n_parity = %d, buflen = %d, count = %d, high = %d, low = %d, roffset = %d\n",
				p2df->n_parity, p2df->buflen, count, p2df->high, p2df->low, p2df->roffset);
	}
	return ret;
}

int par2_decode_file(char *filepath, char *filename)
{
	size_t ret = 0;
	size_t high_read, low_read;
	int i, padlen, result;
	tagpar2_decode_file *p2df;

	p2df = (tagpar2_decode_file *)
		malloc(sizeof(tagpar2_decode_file));
	if(p2df == NULL){
		return PAR2_ERROR_DECODE_FILE_MEMORY;
	}

	result = par2_decode_file_init(p2df, filepath, filename);
	if(result < 0){
		free(p2df);
		return result;
	}

	high_read = low_read = 0;
	padlen = 0;
	while((high_read < p2df->high) || (low_read < p2df->low)){
		for(i=0;i<p2df->n_parity;i++){
			ret = fread(p2df->rbuf+i*p2df->roffset, 1, p2df->roffset, p2df->fi[i]);
			low_read += ret;
			if(low_read < ret){
				high_read++;
			}
			if(ret == 0){
				goto loop_break;
			}
		}
		if(high_read >= p2df->high && low_read >= p2df->low){
			//繰り下げが起きても気にしない。
			padlen = low_read - p2df->low;
		}
		par2_yokotate(p2df->parity, p2df->rbuf, p2df->n_parity, p2df->roffset);
		par2_decode_data(p2df->data, p2df->parity, p2df->n_parity * ret, p2df->n_parity, p2df->flg);
		fwrite(p2df->data, 1, p2df->n_parity * ret - padlen, p2df->fo);
	}
	loop_break:

	par2_decode_file_free(p2df->fi, p2df->n_parity, p2df->fo);

	free(p2df);
	return 0;
}

void par2_print(unsigned char **x, int n_parity)
{
	int i, j;
	fprintf(stdout, "--\n");
	for(j=0;j<n_parity;j++){
		for(i=0;i<n_parity;i++){
			fprintf(stdout, "%3d, ", x[j][i]);
		}
			fprintf(stdout, "\n");
	}
}

void par2_E(unsigned char **x, int n_parity)
{
	int i;
	for(i=0;i<n_parity;i++){
		memset(x[i], 0, n_parity);
		x[i][i] = 1;
	}
}

int par2_inverse(unsigned char **inv, unsigned char **x, int n_parity)
{
	int i, j, k;
	unsigned char tmp0, tmp1, tmp2, tmp3;
	unsigned char inv0, inv1, inv2, inv3;

	par2_E(inv, n_parity);

	for(k=0;k<n_parity;k++){
		if(x[k][k] != 1){
			tmp0 = x[k][k];
			for(i=0;i<n_parity;i++){
				x[k][i] = par2_div_8[x[k][i]][tmp0];
				inv[k][i] = par2_div_8[inv[k][i]][tmp0];
			}
		}

		for(j=0;j<n_parity;j++){
			if(j==k)continue;
			inv0 = tmp0 = x[j][k];
			if(tmp0 == 0)continue;
			for(i=0;i<n_parity;i++){
				tmp1 = x[k][i];
				inv1 = inv[k][i];
				tmp2 = par2_mul_8[tmp0][tmp1];
				inv2 = par2_mul_8[inv0][inv1];
				tmp3 = x[j][i];
				inv3 = inv[j][i];
				x[j][i] = par2_add_8[tmp3][tmp2];
				inv[j][i] = par2_add_8[inv3][inv2];
			}
		}
	}

	return 0;
}

int par2_mult(
	unsigned char *data, unsigned char **inv,
	unsigned char *parity, int n_parity)
{
	int i, j;
	unsigned char tmp0, tmp1;

	for(j=0;j<n_parity;j++){
		tmp0 = 0;
		for(i=0;i<n_parity;i++){
			tmp1 = par2_mul_8[parity[i]][inv[j][i]];
			tmp0 = par2_add_8[tmp0][tmp1];
		}
		data[j] = tmp0;
	}
	return 0;
}
