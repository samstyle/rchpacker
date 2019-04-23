#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char uchar;

char* bname = NULL;
char fname[256];
uchar pframe[0x300];	// prev frame data
uchar cframe[0x300];	// curr frame data
uchar xframe[0x300];	// curr ^ prev
uchar pdata[0x1000];	// packed

void help() {
	printf("Usage:\n");
	printf("-o outfile\n");
	printf("-b basename (basename.XXX.rch)\n");
}

int packFrame(uchar* src, uchar* xor, uchar* dst) {
	uchar* st = dst;
	int adr = 0;
//	int zcount;
//	int ncount = 0;
	uchar* ptr;
	while (adr < 0x300) {
		*dst = 0x00;
		while ((xor[adr] == 0x00) && (adr < 0x300)) {
			if (*dst == 0xff) {
				dst++;
				*dst = 0x00;
				dst++;
				*dst = 0x00;
			}
			*dst = *dst + 1;
			adr++;
		}
		dst++;
		if (adr > 0x2ff) break;
		ptr = dst;
		*ptr = 0x00;
		dst++;
		while ((xor[adr] != 0x00) && (adr < 0x300)) {
			if (*ptr == 0xff) {
				*dst = 0x00;
				dst++;
				ptr = dst;
				dst++;
				*ptr = 0x00;
			}
			*ptr = *ptr + 1;
			*dst = src[adr];
			dst++;
			adr++;
		}
	}
	return dst - st;
}

int processFrame(int num) {
	sprintf(fname,"%s.%.3i.rch",bname,num);	// filename XXX.rch
	FILE* file = fopen(fname, "rb");
	if (!file) return 0;
	fread(cframe, 0x300, 1, file);
	fclose(file);
	int cnt;
	for (cnt = 0; cnt < 0x300; cnt++)
		xframe[cnt] = pframe[cnt] ^ cframe[cnt];
	cnt = packFrame(cframe, xframe, pdata);		// cnt = size of frame
	memcpy(pframe, cframe, 0x300);
	return cnt;
}

int main(int ac, char** av) {
	char* oname = NULL;
	int cnt = 1;
	char* parg;
	while (cnt < ac) {
		parg = av[cnt++];
		if (strcmp(parg, "-o") == 0) oname = av[cnt++];
		if (strcmp(parg, "-b") == 0) bname = av[cnt++];
	}
	if (!oname || !bname) {
		help();
		return 1;
	}
	FILE* ofile = fopen(oname, "wb");
	if (!ofile) {
		printf("Can't open output file\n");
		return 2;
	}
	fputc(0x00, ofile);

	memset(pframe, 0, 0x300);
	cnt = processFrame(0);
	fwrite(pdata, cnt, 1, ofile);

	int num = 1;
	while (1) {
		cnt = processFrame(num);
		if (cnt == 0) break;
		fwrite(pdata,cnt,1,ofile);
		num++;
	}

	cnt = processFrame(0);
	fwrite(pdata, cnt, 1, ofile);

	rewind(ofile);
	fputc(num, ofile);
	fclose(ofile);
	return 1;
}
