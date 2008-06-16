10 n=80
20 dim x(100),y(100)
30 for i=1 to n
40 x(i) = rnd(xsize)-1
50 y(i) = rnd(ysize)-1
60 next i
70 xc=xsize/2
80 yc=ysize/2
90 home
100 color 0,0,64:fill
110 for i=1 to n
120 xc=x(i):yc=y(i):gosub 230
130 next i
140 color 0,0,255
150 r=20
160 disc mousex, mousey, r
170 pen 2
180 color 0,0,0
190 circle mousex, mousey, r
200 home: print sleep(.02)
210 update
220 goto 100
230 dx=mousex - xc
240 dy=yc-mousey
250 if dx or dy then a=atn2(dy, dx) else a=0.0
260 pen 5
270 da=.7
280 r=20
290 r2=r*.5
300 color 255,255,255
310 move xc-r*cos(a), yc+r*sin(a)
320 line xc+r*cos(a), yc-r*sin(a)
330 line xc+r2*cos(a+da), yc-r2*sin(a+da)
340 move xc+r*cos(a), yc-r*sin(a)
350 line xc+r2*cos(a-da), yc-r2*sin(a-da)
360 return
