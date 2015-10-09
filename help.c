/*
   SDL_basic written by David Ashley, released 20080621 under the GPL
   http://www.linuxmotors.com/SDL_basic
   dashxdr@gmail.com
*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

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
{"sgn", 0, "SGN(x) returns 1, -1 or 0 depending on the sign of the argument.\n"
		"sgn(0) is 0.  sgn(50) is 1. sgn(-10) is -1.\n"},
{"rnd", 0, "RND(x) returns a random number.\n"
		"rnd(0) returns a random real number between 0 and 1\n"
		"rnd(50) returns a random integer between 1 and 50\n"},
{"sin", 0, "SIN(x) returns the sine of the angle argument. The angle is in radians.\n"
		"sin(1.570796) is 1.0\n"},
{"cos", 0, "COS() returns the cosine of the angle argument. The angle is in radians.\n"
		"cos(1.570796) is 0.0\n"},
{"log", 0, "LOG(x) returns the logarithm, base e, of the argument.\n"},
{"exp", 0, "EXP(x) returns e raised to the power of the argument.\n"},
{"sqr", 0, "SQR(x) returns the square root of the argument.\n"},
{"pow", 0, "POW(x,y) returns x raised to the power of y.\n"},
{"int", 0, "INT(x) removes the fractional part of the number, always moving towards the next lowest integer\n"},
{"tan", 0, "TAN(x) returns the tangent of the angle argument. The angle is in radians.\n"},
{"atn", 0, "ATN(x) returns the arctangent in radians of the numeric argument.\n"},
{"atn2", 0, "ATN2(y, x) takes two numeric arguments, and returns the angle between the line from (0,0) and (1,0).\n"
		"  The first argument is the 'y' value, second is the 'x' value.\n"},
{"abs", 0, "ABS(x) returns the absolute value of the numeric argument.\n"},
{"key", 0, "KEY(x) returns 1 if the specified keycode is currently pressed, 0 otherwise\n"},
{"note", 0, "NOTE(x) returns the MIDI key frequency corresponding to the note number.\n"
		"  note(69) returns 440.0\n"},
{"len", 0, "LEN(a$) returns the length of the string argument.\n"},
{"val", 0, "VAL(a$) returns the decimal value of the string argument.\n"
		"val(\"1.23\") is 1.23.\n"},
{"asc", 0, "ASC(a$) returns the ascii value of the first character in the string argument.\n"
		"  asc(\"0\") returns 48.\n"},
{"mousex", 0, "MOUSEX in an expression evaluates to the current mouse X position\n"},
{"mousey", 0, "MOUSEY in an expression evaluates to the current mouse Y position\n"},
{"mouseb", 0, "MOUSEB in an expression evaluates to the current mouse button state\n"},
{"xsize", 0, "XSIZE in an expression evaluates to the BASIC window's horizontal size\n"},
{"ysize", 0, "XSIZE in an expression evaluates to the BASIC window's vertical size\n"},
{"ticks", 0, "TICKS in an expression evaluates to the number of elapsed ticks since the\n"
		"  program started. There are 1000 ticks per second.\n"},
{"keycode", 0, "KEYCODE in an expression evaluates to the keycode of the next key pressed\n"
		"  by the user, or -1 if no key was pressed\n"},
{"inkey", 0, "INKEY$ in an expression evaluates to a string containing the next key pressed\n"
		"  by the user, or an empty string if no key was pressed\n"},
{"left", 0, "LEFT$(a$, <expr>) returns the leftmost N characters of the string, as determined\n"
		"  by the value of the numeric expression\n"},
{"right", 0, "RIGHT$(a$, <expr>) returns the rightmost N characters of the string, as determined\n"
		"  by the value of the numeric expression\n"},
{"midstring", 0, "MID$(a$, <expr>, <expr>) returns a string containing the middle characters of\n"
		"  the string. The first expression selects the start, and the second expression selects\n"
		"  the length.\n"},
{"chr", 0, "CHR$(<expr>) returns a string containing a single character with the ASCII code\n"
		"  of the numeric argument.\n"},
{"str", 0, "STR$(<expr>) returns a string representing the decimal value of the\n"
		"  numeric argument.\n"},
{"string", 0, "STRING$(<expr>, a$) returns a string consisting of N of the first characters of\n"
		"  the string argument, as specified by the first argument.\n"
		"  string$(8, \"x\") would evaluate to \"xxxxxxxx\"\n"},
{"fix", 0, "FIX(x) removes the fractional part of the number, always moving towards 0\n"},
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
{"sleep", 0, "SLEEP <seconds>\n"
		"  Sleep for some amount of seconds.\n"
		"  Example: sleep .75      = Sleep for 3/4 of a second.\n"
		"  sleep(x) can also be used in expressions, the argument is how long to wait in seconds,\n"
		"  and the value returned is the amount actually waited, in order to align the runtime clock.\n"},
{"pen", 0, "PEN <expr>\n"
		"  Sets the pen drawing size. Default size is 1 pixel.\n"},
{"color", 0, "COLOR <red>, <green>, <blue> [,<alpha>]\n"
		"  Sets the current drawing color. The parameters are for red, green, blue,\n"
		"  and an optional alpha value. The range for all is from 0 to 255.\n"
		"  Example:  color 255,0,0:fill     = Fills the screen with red.\n"},
{"cls", 0, "CLS\n"
		"  Clears the screen, moves the cursor up to the upper left.\n"},
{"home", 0, "HOME\n"
		"  Moves the cursor to the upper left.\n"},
{"circle", 0, "CIRCLE <xpos>, <ypos>, <radius>\n"
		"  Parameters are the x position, y position, and radius. Draws a circle\n"
		"  with the current pen color and size.\n"
		"  Example:  circle 400,400,200\n"},
{"disc", 0, "DISC <xpos>, <ypos>, <radius>\n"
		"  Parameters are the x position, y position, and the radius. Draws a solid\n"
		"  circular disc with the current pen color and size.\n"
		"  Example:  disc 400,400,200\n"},
{"fill", 0, "FILL\n"
		"  Fills the display with the current color.\n"},
{"move", 0, "MOVE <xpos>, <ypos>\n"
		"  Move the drawing pen to the specified X and Y positions. No drawing is done.\n"
		"  Example:   move 100,100\n"},
{"line", 0, "LINE <xpos>, <ypos>\n"
		"  Draws a line from the current position to the specified X and Y position.\n"
		"  Example:   line 300,200\n"},
{"box", 0, "BOX <xpos>, <ypos>, <xext>, <yext> [modifiers]\n"
		"  Draws a solid rectangular box. The parameters are X position, Y position,\n"
		"  X extension, Y extension. The coordinates define the center of the box.\n"
		"  The extensions represent half the width or height of the resultant box.\n"
		"  See the help on 'modifiers'\n"},
{"rect", 0, "RECT <xpos>, <ypos>, <xext>, <yext> [modifiers]\n"
		"  Draws a rectangle. The parameters are X position, Y position,\n"
		"  X extension, Y extension. The coordinates define the center of the rectangle.\n"
		"  The extensions represent half the width or height of the resultant rectangle.\n"
		"  See the help on 'modifiers'\n"},
{"modifiers", 0, "[modifiers]\n"
		"  Some drawing commands, such as BOX or RECT, can accept one or more\n"
		"  modifiers. Modifiers can be:\n"
		"  round <expr>        ; For BOX or RECT specifies radius of rounded corners\n"
		"  rotate <expr>       ; For BOX or RECT specifies the rotation (not implemented)\n"
		"  EXAMPLE: box 500,500,250,50 round 30\n"},
{"arc", 0, "ARC <xpos>, <ypos>, <radius>, <angle>, <anglesize>\n"
		"  Draws an arc. The parameters are X position, Y position,\n"
		"  radius, starting angle, and angle size. The angle and size\n"
		"  are in degrees, and both can be positive or negative.\n"},
{"wedge", 0, "WEDGE <xpos>, <ypos>, <inrad>, <outrad>, <angle>, <anglesize>\n"
		"  Draws a pie slice-like wedge. Parameters are X position, Y position\n"
		"  inner radius, outer radius, starting angle, and angle size. The\n"
		"  angle and size are in degrees, and both can be positive or negative.\n"},
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
{"on", 0, "ON <expr> GOTO <line#>[,<line#>]...\n"
		"ON <expr> GOSUB <line#>[,<line#>]...\n"
		"  Evaluate the expression, then either goto or gosub to the line number\n"
		"  in the matching position in the list. The numbers start from 1.\n"
		"  Example:   on i goto 100,150,200,250\n"},
{"input", 0, "INPUT <var>[,<var>]...\n"
		"INPUT <string literal>;<var>[,<var>]...\n"
		"  Used for entering numbers or strings from the user. Examples:\n"
		"  input \"Type in a number between 1 and 10\";n\n"
		"  input \"Please answer yes or no\";a$\n"
		"  input x,y\n"},
{"read", 0, "READ...DATA\n"
		"Use DATA statements to provide a list of numbers or strings.\n"
		"Use READ to define variables from the data. Examples:\n"
		"10 data 2,3,5,7,11,13\n"
		"20 read p(1), p(2), p(3), p(4), p(5), p(6)\n"
		"...in the end, p(1) will be 2, p(2) will be 3, p(3) will be 5...and so on.\n"},
{"data", "read", ""},
{"quiet", 0, "QUIET <expr>\n"
		"QUIET with no arguments turns off all sound voices. Quiet with a numeric\n"
		"argument turns off that voice only. Example:\n"
		"quiet 5      = turn off voice #5\n"},
{"tone", 0, "TONE <expr>[,freq <expr>][,dur <expr>][,fmul <expr>][,vol <expr>]\n"
		"          [,wsqr][,wsin][,wsaw][,wtri]\n"
		"  Start a sound voice. The first argument is the voice number, currently\n"
		"  from 1 to 32. Voices 28 to 32 are noise channels. Voices 1 through 27\n"
		"  are tones. The tone parameters can be set as follows:\n"
		"  freq    =  Set the frequency in Hz.\n"
		"  dur     =  Set the duration in seconds.\n"
		"  fmul    =  Set the frequency multiplier. Set to 1 and the frequency\n"
		"             remains constant. Values greater than 1 will cause the frequency\n"
		"             to rise. Values less than 1 will cause the frequency to fall.\n"
		"  vol     =  Set the volume from 1 to 100\n"
		"  wsqr    =  Choose a square wave\n"
		"  wsin    =  Choose a sine wave\n"
		"  wsaw    =  Choose a sawtooth wave\n"
		"  wtri    =  Chose a triangular wave\n"
		"  Example:\n"
		"  tone 5, freq 880, wsin, vol 100, dur 1, fmul 1.0001\n"
		"  See also: QUIET\n"},
{"shape", 0, "SHAPE COMMANDS\n"
		"  The shape commands are SHINIT, SHEND, SHDONE, SHLINE, SHCURVE and SHCUBIC.\n"
		"  They provide an interface for a shape rendering engine. The idea is you\n"
		"  define one or more outlines using lines and quadratic or cubic bezier\n"
		"  curves. For example you could draw a donut by tracing an outer circle\n"
		"  then an inner circle. The cool14 demo makes use of the shape interface.\n"
		"  The shape commands are:\n"
		"  SHINIT  = Initialize a shape\n"
		"  SHEND   = Terminate a path so a new one can be started. Paths are\n"
		"            automatically closed if the last point on a path isn't the\n"
		"            same as the first point.\n"
		"  SHDONE  = The current shape is finished, now render it.\n"
		"  SHLINE <x>,<y> = Stroke a line to the point from current position.\n"
		"            It's good practice to follow the SHINIT or SHEND\n"
		"            with at least one SHLINE.\n"
		"  SHCURVE <xc>,<yc>,<x>,<y> = Stroke a quadratic bezier curve from current\n"
		"            position. The <xc>,<yc> is the control point.\n"
		"  SHCUBIC <xc1>,<yc1>,<xc2>,<yc2>,<x>,<y> = Stroke a cubic bezier curve\n"
		"            from current position. Two control points are required.\n"
		"  Example: This program draws a weird figure with a box cutout\n"
		"     10 cls\n"
		"     20 shinit: shline 100,100\n"
		"     30 shcurve 150,50,200,100\n"
		"     40 shline 200,200\n"
		"     50 shcubic 300,220,0,220,100,200\n"
		"     60 shline 100,100\n"
		"     70 shend\n"
		"     80 shline 130,130:shline 170,130\n"
		"     90 shline 170,170:shline 130,170\n"
		"     100 shdone\n"},
{"shinit", "shape", ""},
{"shend", "shape", ""},
{"shdone", "shape", ""},
{"shline", "shape", ""},
{"shcurve", "shape", ""},
{"shcubic", "shape", ""},
{"wsin", "tone", ""},
{"wsaw", "tone", ""},
{"wsqr", "tone", ""},
{"wtri", "tone", ""},
{"loadtexture", 0, "LOADTEXTURE(\"filename.png\") returns an integer associated with the\n"
		"  image file specified. The integer can be used with DRAWTEXTURE.\n"},
{"drawtexture", 0, "DRAWTEXTURE texture_number, x, y\n"
		"  draw texture centered at specified coordinates. See LOADTEXTURE.\n"},
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
help *ht[HTMAX];
int n,i,j, nc, nr;
#define WIDTH 13

	h=helplist;
	for(n=0;n<HTMAX && h->name;++n, ++h)
		ht[n] = h;
	qsort(ht, n, sizeof(ht[0]), hcomp);

	tprintf(bc, "Use: help <subject>\n");
	tprintf(bc, "Help on any of these subjects:\n");
	h=helplist;
	nc = (bc->txsize-1) / WIDTH;
	nr = (n + nc-1)/nc;
	for(i=0;i<nr;++i)
	{
		for(j=i;j<n;j+=nr)
			tprintf(bc, "%-12s", ht[j]->name);
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
