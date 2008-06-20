#include <stdlib.h>
#include "misc.h"

typedef struct {
	char *name;
	char *alias;
	char *info;
} help;

help helplist[]={
{"help", 0, "Help tells you information about BASIC statements or commands.\n"},
{"run", 0, "Runs the current program."},
{"list", 0, "Lists lines in the current program.\n"
		"   list           = list entire program\n"
		"   list 100-      = list all lines from 100 on\n"
		"   list -100      = list all lines up to 100\n"
		"   list 100-200   = list all lines from 100 through 200\n"},
{"for", 0, "FOR...NEXT loops:\n"
		"   for i=1 to 10         = loop through 10 times, i goes 1 through 10\n"
		"   for i=2 to 30 step 2  = loop with optional step -- counts by 2's.\n"
		"   end loops with a 'next i'\n"},
{"next", "for", ""},
{"goto", 0, "Use GOTO to jump to another line in the program\n"
		"goto 100          = Start executing code at line 100\n"},
{"gosub", 0, "Use GOSUB...RETURN to execute a subroutine.\n"
		"gosub 100         = run subroutine at line 100\n"
		"                    subroutine must end in a 'return'\n"},
{"return", "gosub", ""},
{"save", 0, "Save the current BASIC program to the filename specified.\n"
		"save test         = save to file 'test.bas'\n"},
{"load", 0, "Load BASIC program from the filename specified.\n"
		"load test         = load file 'test.bas'\n"},
{"sgn", 0, "SGN() returns 1, -1 or 0 depending on the sign of the argument.\n"
		"sgn(0) is 0.  sgn(50) is 1. sgn(-10) is -1.\n"},
{"rnd", 0, "RND() returns a random number.\n"
		"rnd(0) returns a random real number between 0 and 1\n"
		"rnd(50) returns a random integer between 1 and 50\n"},
{"sin", 0, "SIN() returns the sine of the angle argument. The angle is in radians.\n"
		"sin(1.570796) is 1.0\n"},
{"cos", 0, "COS() returns the cosine of the angle argument. The angle is in radians.\n"
		"cos(1.570796) is 0.0\n"},
{"log", 0, "LOG() returns the logarithm, base e, of the argument.\n"},
{"exp", 0, "EXP() returns e raised to the power of the argument.\n"},
{"sqr", 0, "SQR() returns the square root of the argument.\n"},
{"pow", 0, "POW(x,y) returns x raised to the power of y.\n"},
{"int", 0, "INT() removes the fractional part of the number, always moving towards the next lowest integer\n"},
{"fix", 0, "FIX() removes the fractional part of the number, always moving towards 0\n"},
{"let", 0, "LET <var>=<expr>\n"
			"  The LET is optional. Assigns value of <expr> to the <var>.\n"},
{"if", 0, "IF <expr> THEN <statements> ELSE <statements>\n"
		"  If the <expr> is non-zero (true) the first <statements> are executed.\n"
		"  If the <expr> is zero (false), the second <statements> are executed.\n"
		"  The ELSE and the <statements> after it are optional.\n"
		"  Statements my be separated by the ':' character.\n"},
{"then", "if", ""},
{"else", "if", ""},
{"print", 0, "PRINT <expr>[, or ;] ...\n"
		"PRINT @<expr> ...\n"
		"PRINT tab(<expr>) ...\n"
		"  Prints out information. The <expr>'s can be strings or numeric expressions.\n"
		"  @<expr> is for TRS-80 compatibility -- print at the screen location on 64x16 display.\n"
		"  tab(<expr>) prints at the specified column\n"
		"  The ',' or ';' separators affect spacing between printed elements.\n"
		"  The ',' tabs to 16 spaces. The ';' doesn't move the print position.\n"
		"  Ending with either ',' or ';' allows more printing on the same line.\n"},
{"dim", 0, "DIM <var>(<size_list>)\n"
		" Allocate an array of storage. The <size_list> can have up to 16 dimensions,\n"
		" separated by commas.\n"
		" Examples: dim a(10,10), x$(20)\n"},
{"end", 0, "END means end the current program.\n"},
{"stop", 0, "STOP means stop execution at this point, as if control-C or escape\n"
		"  had been hit.\n"},
{"sleep", 0, "SLEEP <expr>\n"
		"  Sleep for some amount of seconds.\n"
		"  Example: sleep .75      = Sleep for 3/4 of a second.\n"},
{"pen", 0, "PEN <expr>\n"
		"  Sets the pen drawing size. Default size is 1 pixel.\n"},
{"color", 0, "COLOR <expr>, <expr>, <expr> [,<expr>]\n"
		"  Sets the current drawing color. The parameters are for red, green, blue,\n"
		"  and an optional alpha value. The range for all is from 0 to 255.\n"
		"  Example:  color 255,0,0:fill     = Fills the screen with red.\n"},
{"cls", 0, "CLS\n"
		"  Clears the screen, moves the cursor up to the upper left.\n"},
{"home", 0, "HOME\n"
		"  Moves the cursor to the upper left.\n"},
{"circle", 0, "CIRCLE <expr>, <expr>, <expr>\n"
		"  Parameters are the x position, y position, and radius. Draws a circle\n"
		"  with the current pen color and size.\n"
		"  Example:  circle 400,400,200\n"},
{"disc", 0, "DISC <expr>, <expr>, <expr>\n"
		"  Parameters are the x position, y position, and the radius. Draws a solid\n"
		"  circular disc with the current pen color and size.\n"
		"  Example:  disc 400,400,200\n"},
{"fill", 0, "FILL\n"
		"  Fills the display with the current color.\n"},
{"move", 0, "MOVE <expr>, <expr>\n"
		"  Move the drawing pen to the specified X and Y positions. No drawing is done.\n"
		"  Example:   move 100,100\n"},
{"line", 0, "LINE <expr>, <expr>\n"
		"  Draws a line from the current position to the specified X and Y position.\n"
		"  Example:   line 300,200\n"},
{"box", 0, "BOX <expr>, <expr>, <expr>, <expr>\n"
		"  Draws a solid rectangular box. The parameters are X position, Y position\n"
		"  X extension, Y extension. The coordinates define the center of the box.\n"
		"  The extensions represent half the width or height of the resultant box.\n"},
{"rect", 0, "RECT <expr>, <expr>, <expr>, <expr>\n"
		"  Draws a rectangle. The parameters are X position, Y position\n"
		"  X extension, Y extension. The coordinates define the center of the rectangle.\n"
		"  The extensions represent half the width or height of the resultant rectangle.\n"},
{"spot", 0, "SPOT\n"
		"  Draws a circular spot at the current drawing position with the current color\n"
		"  and pen size.\n"},
{"update", 0, "UPDATE\n"
		"  Normally screen updates take place automatically about 50 times per\n"
		"  second. Sometimes you're in the middle of drawing when the update occurs\n"
		"  so you see flickering of items, or the drawing isn't complete. Use UPDATE\n"
		"  to control when the display gets updated. For about 1/4 second after using\n"
		"  the UPDATE command, the auto update of the display will be disabled.\n"},
{"rem", 0, "REM <comments>\n"
		"  REM lets you introduce comments into the code. BASIC ignores the rest of the\n"
		"  line after a REM.\n"
		"  Example:  if j=1 then gosub 100: REM **** Draw the rocket\n"},

{0,0}
};

int hcomp(const void *p1, const void *p2)
{
	return strcmp((*(help **)p1)->name, (*(help **)p2)->name);
}

#define HTMAX 1000
void listhelp(bc *bc, char *p)
{
help *h;
int online;
help *ht[HTMAX];
int n,i,j, nc, nr;
#define WIDTH 10

	h=helplist;
	for(n=0;n<HTMAX && h->name;++n, ++h)
		ht[n] = h;
	qsort(ht, n, sizeof(ht[0]), hcomp);

	tprintf(bc, "Use: help <subject>\n");
	tprintf(bc, "Help on any of these subjects:\n");
	h=helplist;
	nc = (bc->txsize-1) / WIDTH;
	nr = (n + nc-1)/nc;
	online=0;
	for(i=0;i<nr;++i)
	{
		for(j=i;j<n;j+=nr)
			tprintf(bc, "%-10s", ht[j]->name);
		tprintf(bc, "\n");
	}
}

help *findhelp(char *p)
{
int d=0;
help *h;
int i, len;

	for(;;)
	{
		h=helplist;
		len=strlen(p);
		while(h->name)
		{
			for(i=0;i<len;++i)
				if(tolower(p[i]) != h->name[i])
					break;
				else if(!p[i])
					break;
			if(!p[i])
				break;
			++h;
		}
		if(!h->name)
			break;
		if(!d++ && h->alias)
		{
			p=h->alias;
			continue;
		}
		return h;
	}
	return 0;
}

void dohelp(bc *bc, char *p)
{
help *h;

	if(!*p)
		listhelp(bc, p);
	else
	{
		h=findhelp(p);
		if(h && h->name)
			tprintf(bc, "\n%s:\n\n%s\n", p, h->info);
		else
			tprintf(bc, "No help available on '%s'\n", p);
	}

}
