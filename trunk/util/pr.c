/* Quick program to dump a TRS-80 basic file */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void setup(char **keyword)
{
	keyword[0] = "END ";		keyword[1] = "FOR ";
	keyword[2] = "RESET";		keyword[3] = "SET";
	keyword[4] = "CLS ";		keyword[5] = "CMD ";
	keyword[6] = "RANDOM ";		keyword[7] = "NEXT ";
	keyword[8] = "DATA ";		keyword[9] = "INPUT ";
	keyword[10] = "DIM ";		keyword[11] = "READ ";
	keyword[12] = "LET ";		keyword[13] = " GOTO ";
	keyword[14] = "RUN ";		keyword[15] = "IF ";
	keyword[16] = "RESTORE ";	keyword[17] = " GOSUB ";
	keyword[18] = "RETURN ";	keyword[19] = "REM ";
	keyword[20] = "STOP ";		keyword[21] = " ELSE ";
	keyword[22] = "TRON ";		keyword[23] = "TROFF ";
	keyword[24] = "DEFSTR ";	keyword[25] = "DEFINT ";
	keyword[26] = "DEFSNG ";	keyword[27] = "DEFDBL ";
	keyword[28] = "LINE ";		keyword[29] = "EDIT ";
	keyword[30] = "ERROR ";		keyword[31] = "RESUME ";
	keyword[32] = "OUT ";		keyword[33] = "ON ";
	keyword[34] = "OPEN ";		keyword[35] = "FIELD ";
	keyword[36] = "GET ";		keyword[37] = "PUT ";
	keyword[38] = "CLOSE ";		keyword[39] = "LOAD ";
	keyword[40] = "MERGE ";		keyword[41] = "NAME ";
	keyword[42] = "KILL ";		keyword[43] = "LSET ";
	keyword[44] = "RSET ";		keyword[45] = "SAVE ";
	keyword[46] = "SYSTEM ";	keyword[47] = "LPRINT ";
	keyword[48] = "DEF ";		keyword[49] = "POKE ";
	keyword[50] = "PRINT ";		keyword[51] = "CONT ";
	keyword[52] = "LIST ";		keyword[53] = "LLIST ";
	keyword[54] = "DELETE ";	keyword[55] = "AUTO ";
	keyword[56] = "CLEAR ";		keyword[57] = "CLOAD ";
	keyword[58] = "CSAVE ";		keyword[59] = "NEW ";
	keyword[60] = "TAB(";		keyword[61] = " TO ";
	keyword[62] = "FN ";		keyword[63] = "USING ";
	keyword[64] = "VARPTR ";	keyword[65] = "USR ";
	keyword[66] = "ERL ";		keyword[67] = "ERR ";
	keyword[68] = " STRING$";	keyword[69] = " INSTR";
	keyword[70] = " POINT";		keyword[71] = " TIME$ ";
	keyword[72] = " MEM ";		keyword[73] = " INKEY$ ";
	keyword[74] = " THEN ";		keyword[75] = " NOT ";
	keyword[76] = " STEP ";		keyword[77] = "+";
	keyword[78] = "-";			keyword[79] = "*";
	keyword[80] = "/";			keyword[81] = "~";
	keyword[82] = " AND ";		keyword[83] = " OR ";
	keyword[84] = ">";			keyword[85] = "=";
	keyword[86] = "<";			keyword[87] = " SGN";
	keyword[88] = " INT";		keyword[89] = " ABS";
	keyword[90] = " FRE";		keyword[91] = " INP";
	keyword[92] = " POS";		keyword[93] = " SQR";
	keyword[94] = " RND";		keyword[95] = " LOG";
	keyword[96] = " EXP";		keyword[97] = " COS";
	keyword[98] = " SIN";		keyword[99] = " TAN";
	keyword[100] = " ATN";		keyword[101] = " PEEK";
	keyword[102] = " CVI";		keyword[103] = " CVS";
	keyword[104] = " CVD";		keyword[105] = " EOF";
	keyword[106] = " LOC";		keyword[107] = " LOF";
	keyword[108] = " MKI$";		keyword[109] = " MKS$";
	keyword[110] = " MKD$";		keyword[111] = " CINT";
	keyword[112] = " CSNG";		keyword[113] = " CDBL";
	keyword[114] = " FIX";		keyword[115] = " LEN";
	keyword[116] = " STR$";		keyword[117] = " VAL";
	keyword[118] = " ASC";		keyword[119] = " CHR$";
	keyword[120] = " LEFT$";	keyword[121] = " RIGHT$";
	keyword[122] = " MID$";		keyword[123] = " ' ";
}

int main(int argc, char **argv)
{
char *keyword[128]={0};
int c;
int fd;
unsigned char buffer[65536];
int len;
unsigned char *p, *e;

	if(argc<2)
	{
		printf("Specify filename\n");
		exit(0);
	}
	fd=open(argv[1], O_RDONLY);
	if(fd<0)
	{
		printf("Failed to open\n");
		exit(-1);
	}
	len=read(fd, buffer, sizeof(buffer));
	close(fd);
	if(len<0)
	{
		printf("Error reading\n");
		exit(-2);
	}
	e=buffer+len;
	p=buffer;

	setup(keyword);
	if(p<e && *p!=0xff)
	{
		printf("Not a basic file\n");
		exit(-3);
	}
	++p;
	for(;;)
	{
		if(p+2>e) break; // need 2 byres for flags
		if(!p[0] && !p[1]) break;
		p+=2;
		if(p+2>e) break; // need 2 bytes for line number
		printf("%d ", p[0]+(p[1]<<8));
		p+=2;
		while(p<e && (c=*p++))
		{
			if(c<128)
				putchar(c);
			else
			{
				c-=128;
				if(keyword[c])
					printf("%s", keyword[c]);
			}
		}
		printf("\n");

	}

	return 0;
}
