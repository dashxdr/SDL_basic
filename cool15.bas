10 rem *** Inspired by inkscape's polygon tool
20 control1 = 0
30 control2 = 2
40 halfpi = 3.1415927/2.0
50 twopi = 3.1415927*2.0
60 px = xsize/2:py = ysize/2: dpx = 4: dpy = 4.2737127283
70 px2 = xsize/32: py2 = ysize*.5: dpx2 = 3.77282848: dpy2 = 4
80 cx = xsize/2
90 cy = ysize/2
100 n=20
110 r = ysize*.5
120 rem ****** Main loop
130 ia2 = 3.1415927*2.0*(px-xsize/2)/xsize:cd = (xsize/2)*(py-ysize/2)/ysize
140 r2 = r*px2/xsize: n = 3+int(50*py2/ysize):da = 3.1415927*2.0/n
150 gosub 650: rem handle mouse control toggles
160 gosub 450: rem handle control1
170 gosub 550: rem handle control2
180 a = -da/2
190 a2=ia2
200 color 255,255,255
210 fill
220 gosub 720
230 goto 380
240 shinit
250 shline cx+r2*cos(a2-da), cy-r2*sin(a2-da)
260 for i=1 to n
270 x2 = cx+r2*cos(a2):y2 = cy-r2*sin(a2)
280 x1 = cx+r*cos(a):y1 = cy-r*sin(a)
290 tx1 = cd*cos(a-halfpi):ty1 = -cd*sin(a-halfpi)
300 tx2 = cd*cos(a2-halfpi):ty2 = -cd*sin(a2-halfpi)
310 shcubic cx+r2*cos(a2-da) - cd*cos(a2-da-halfpi), cy-r2*sin(a2-da) + cd*sin(a2-da-halfpi), x1+tx1, y1+ty1, x1, y1
320 shcubic x1-tx1, y1-ty1, x2 + tx2, y2 + ty2, x2, y2
330 a = a + da
340 a2 = a2 + da
350 next i
360 color 255,0,0
370 shdone
380 color 0,255,0
390 disc px, py, 5
400 color 0,0,255
410 disc px2, py2, 5
420 update
430 sleep .04
440 goto 120
450 rem *******  control1 handler
460 if control1 <> 0 then 530
470 px = px + dpx
480 if px<=0 then px = 0:dpx = -dpx
490 if px>=xsize then px = xsize-1:dpx = -dpx
500 py = py + dpy
510 if py<=0 then py = 0: dpy = -dpy
520 if py>=ysize then py = ysize-1:dpy = -dpy
530 if control1=1 then px=mousex:py=mousey
540 return
550 rem ******   control2 handler
560 if control2 <> 0 then 630
570 px2 = px2 + dpx2
580 if px2<=0 then px2 = 0: dpx2 = -dpx2
590 if px2>=xsize then px2 = xsize-1: dpx2 = -dpx2
600 py2 = py2 + dpy2
610 if py2<=0 then py2 = 0: dpy2 = -dpy2
620 if py2>=ysize then py2 = ysize-1: dpy2 = -dpy2
630 if control2=1 then px2=mousex:py2=mousey
640 return
650 rem ***** Mouse control on/off
660 newmb = mouseb
670 bits = ~oldmb & newmb
680 oldmb = newmb
690 if bits&1 then control1 = control1 + 1: if control1 = 3 then control1 = 0
700 if bits&4 then control2 = control2 + 1: if control2 = 3 then control2 = 0
710 return
720 rem ***** spirograph
730 home
735 px = 445: py = 218
740 v1 = int(px)
750 v2 = int(py)
760 if v1=0 or v2=0 then return
770 if v2>v1 then t=v1:v1=v2:v2=t
780 t=v1 mod v2
790 if t>0 then v1=t:goto 770
800 n = px*py/v2
810 print px, py, v2, n
820 v1 = int(px)
830 v2 = int(py)
840 color 255,0,0
850 shinit
860 for i=1 to n
870 a1 = twopi*(i mod v1)/v1
880 a2 = twopi*(i mod v2)/v2
890 r1 = xsize/4
900 r2 = r1/2
910 cx = xsize/2
920 cy = ysize/2
930 x = cx + r1*cos(a1) + r2*cos(a2)
940 y = cy - r1*sin(a1) - r2*sin(a2)
950 shline x, y
960 if i=1 or i=n then print a1,a2,x,y
970 next i
980 shdone
990 return
