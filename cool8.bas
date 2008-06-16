10 n=80
20 dim x(100),y(100)
30 for i=1 to n
40 x(i) = rnd(xsize)-1
50 y(i) = rnd(ysize)-1
60 next i
70 xc=xsize/2
80 yc=ysize/2
90 home
100 cls
110 for i=1 to n
120 xc=x(i):yc=y(i):gosub 190
130 next i
140 color 0,0,255
150 disc mousex, mousey, 20
160 home: print sleep(.02)
170 update
180 goto 100
190 dx=mousex - xc
200 dy=yc-mousey
210 if dx or dy then a=atn2(dy, dx) else a=0.0
220 pen 5
230 da=.7
240 r=20
250 r2=r*.5
260 color 255,255,255
270 move xc-r*cos(a), yc+r*sin(a)
280 line xc+r*cos(a), yc-r*sin(a)
290 line xc+r2*cos(a+da), yc-r2*sin(a+da)
300 move xc+r*cos(a), yc-r*sin(a)
310 line xc+r2*cos(a-da), yc-r2*sin(a-da)
320 return
