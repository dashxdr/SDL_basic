10 n=80
20 dim x(100),y(100)
30 for i=1 to n
40 x(i) = rnd(xsize)-1
50 y(i) = rnd(ysize)-1
60 next i
70 xc=xsize/2
80 yc=ysize/2
90 color 0,0,64:fill
100 gosub 310
110 for i=1 to n
120 xc=x(i):yc=y(i):gosub 170
130 next i
140 home: print sleep(.02)
150 update
160 goto 90
170 dx=mousex - xc
180 dy=yc-mousey
190 if dx or dy then a=atn2(dy, dx) else a=0.0
200 pen 5
210 da=.7
220 r=20
230 r2=r*.5
240 color 255,255,255
250 move xc-r*cos(a), yc+r*sin(a)
260 line xc+r*cos(a), yc-r*sin(a)
270 line xc+r2*cos(a+da), yc-r2*sin(a+da)
280 move xc+r*cos(a), yc-r*sin(a)
290 line xc+r2*cos(a-da), yc-r2*sin(a-da)
300 return
310 r=20
320 color 0,0,255
330 disc mousex, mousey, r
340 pen 2
350 color 0,0,0
360 circle mousex, mousey, r
370 return
