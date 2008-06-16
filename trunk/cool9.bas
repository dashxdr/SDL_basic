10 pi2=3.1415928*2.0
20 x = xsize/2 : rem x position of ship
30 y = ysize/2 : rem y position of ship
40 thrust=.2 : rem thrust
50 v=0: rem velocity
60 mv=7 : rem max velocity
70 da = .1: rem rotation speed
80 a=3.1415928/2
90 rem ************************   Main loop
100 cls
110 fc=fc+1
120 gosub 170
130 gosub 600
140 print sleep(.02);
150 update
160 goto 90
170 rem ************************   Handle ship movement
180 fire=0
190 if key(402)=0 then 250
200 dx = dx + thrust*cos(a)
210 dy = dy - thrust*sin(a)
220 v = sqr(dx*dx + dy*dy)
230 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
240 fire = 1
250 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
260 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
270 x=x+dx
280 if x<0 then x=x+xsize
290 if x>= xsize then x=x-xsize
300 y=y+dy
310 if y<0 then y=y+ysize
320 if y>=ysize then y=y-ysize
330 gosub 410
340 if x>xsize/2 then fx=-xsize else fx=xsize
350 if y>ysize/2 then fy=-ysize else fy=ysize
360 x=x+fx:gosub 410
370 y=y+fy:gosub 410
380 x=x-fx:gosub 410
390 y=y-fy
400 return
410 rem ************************   Draw ship
420 pen 2
430 as=2.5
440 r=20
450 r2=r*.8
460 color 255,255,255
470 move x+r*cos(a), y-r*sin(a)
480 line x+r2*cos(a+as), y-r2*sin(a+as)
490 line x+r2*cos(a-as), y-r2*sin(a-as)
500 line x+r*cos(a), y-r*sin(a)
510 if fire=0 then 590
520 if fc&1 = 0 then 590
530 r2=r*.7
540 move x-r2*cos(a), y+r2*sin(a)
550 r2=r2*2
560 color 255,190,0
570 pen 4
580 line x-r2*cos(a), y+r2*sin(a)
590 return
600 rem ************************  Hacks
610 if key(13) then x=xsize/2:y=ysize/2:dx=0:dy=0
620 return
