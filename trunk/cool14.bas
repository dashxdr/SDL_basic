10 rem *** Inspired by inkscape's polygon tool
20 px = 0:py = 0: dpx = 4: dpy = 4.2737127283
30 cx = xsize/2
40 cy = ysize/2
50 n=20
60 r = ysize*.5
70 r2 = r*.3
80 da = 3.1415927*2.0/n
90 cd = .2
100 ia=0:ia2=da/2
110 a=ia
120 a2=ia2
130 color 255,255,255
140 fill
150 color 255,0,0
160 shinit
170 shline cx+r2*cos(a2-da), cy-r2*sin(a2-da)
180 for i=1 to n
190 shcubic cx+r2*cos(a2-da+cd), cy-r2*sin(a2-da+cd), cx+r*cos(a-cd), cy-r*sin(a-cd), cx+r*cos(a), cy-r*sin(a)
200 shcubic cx+r*cos(a+cd),cy-r*sin(a+cd), cx+r2*cos(a2-cd), cy-r2*sin(a2-cd), cx+r2*cos(a2), cy-r2*sin(a2)
210 a = a + da
220 a2 = a2 + da
230 next i
240 shdone
250 color 0,0,0
260 disc px, py, 5
270 update
280 px = px + dpx
290 if px<=0 then px = 0:dpx = -dpx
300 if px>=xsize then px = xsize-1:dpx = -dpx
310 py = py + dpy
320 if py<=0 then py = 0: dpy = -dpy
330 if py>=ysize then py = ysize-1:dpy = -dpy
340 rem ***** Uncomment the line below to allow mouse control
350 rem px=mousex:py=mousey
360 ia2 = 3.1415927*2.0*px/xsize:cd = 3.1415927*py/ysize
370 sleep .04
380 goto 110
