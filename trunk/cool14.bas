10 rem *** Inspired by inkscape's polygon tool
20 control1 = 0
30 control2 = 2
40 halfpi = 3.1415927/2.0
50 px = xsize/2:py = ysize/2: dpx = 4: dpy = 4.2737127283
60 px2 = xsize/32: py2 = ysize*.5: dpx2 = 3.77282848: dpy2 = 4
70 cx = xsize/2
80 cy = ysize/2
90 n=20
100 r = ysize*.5
110 rem ****** Main loop
120 ia2 = 3.1415927*2.0*(px-xsize/2)/xsize:cd = (xsize/2)*(py-ysize/2)/ysize
130 r2 = r*px2/xsize: n = 3+int(50*py2/ysize):da = 3.1415927*2.0/n
140 gosub 620: rem handle mouse control toggles
150 gosub 420: rem handle control1
160 gosub 520: rem handle control2
170 a = -da/2
180 a2=ia2
190 color 255,255,255
200 fill
210 shinit
220 shline cx+r2*cos(a2-da), cy-r2*sin(a2-da)
230 for i=1 to n
240 x2 = cx+r2*cos(a2):y2 = cy-r2*sin(a2)
250 x1 = cx+r*cos(a):y1 = cy-r*sin(a)
260 tx1 = cd*cos(a-halfpi):ty1 = -cd*sin(a-halfpi)
270 tx2 = cd*cos(a2-halfpi):ty2 = -cd*sin(a2-halfpi)
280 shcubic cx+r2*cos(a2-da) - cd*cos(a2-da-halfpi), cy-r2*sin(a2-da) + cd*sin(a2-da-halfpi), x1+tx1, y1+ty1, x1, y1
290 shcubic x1-tx1, y1-ty1, x2 + tx2, y2 + ty2, x2, y2
300 a = a + da
310 a2 = a2 + da
320 next i
330 color 255,0,0
340 shdone
350 color 0,255,0
360 disc px, py, 5
370 color 0,0,255
380 disc px2, py2, 5
390 update
400 sleep .04
410 goto 110
420 rem *******  control1 handler
430 if control1 <> 0 then 500
440 px = px + dpx
450 if px<=0 then px = 0:dpx = -dpx
460 if px>=xsize then px = xsize-1:dpx = -dpx
470 py = py + dpy
480 if py<=0 then py = 0: dpy = -dpy
490 if py>=ysize then py = ysize-1:dpy = -dpy
500 if control1=1 then px=mousex:py=mousey
510 return
520 rem ******   control2 handler
530 if control2 <> 0 then 600
540 px2 = px2 + dpx2
550 if px2<=0 then px2 = 0: dpx2 = -dpx2
560 if px2>=xsize then px2 = xsize-1: dpx2 = -dpx2
570 py2 = py2 + dpy2
580 if py2<=0 then py2 = 0: dpy2 = -dpy2
590 if py2>=ysize then py2 = ysize-1: dpy2 = -dpy2
600 if control2=1 then px2=mousex:py2=mousey
610 return
620 rem ***** Mouse control on/off
630 newmb = mouseb
640 bits = ~oldmb & newmb
650 oldmb = newmb
660 if bits&1 then control1 = control1 + 1: if control1 = 3 then control1 = 0
670 if bits&4 then control2 = control2 + 1: if control2 = 3 then control2 = 0
680 return
