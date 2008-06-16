10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100)
20 missnum = 0
30 pi2=3.1415928*2.0
40 x = xsize/2 : rem x position of ship
50 y = ysize/2 : rem y position of ship
60 thrust=.2 : rem thrust
70 v=0: rem velocity
80 mv=7 : rem max velocity
90 da = .1: rem rotation speed
100 a=3.1415928/2
110 rem ************************   Main loop
120 cls
130 fc=fc+1
140 gosub 200: rem handle ship
150 gosub 630: rem hacks
160 gosub 680: rem missiles
170 print sleep(.02);
180 update
190 goto 110
200 rem ************************   Handle ship movement
210 fire=0
220 if key(402)=0 then 280
230 dx = dx + thrust*cos(a)
240 dy = dy - thrust*sin(a)
250 v = sqr(dx*dx + dy*dy)
260 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
270 fire = 1
280 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
290 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
300 x=x+dx
310 if x<0 then x=x+xsize
320 if x>= xsize then x=x-xsize
330 y=y+dy
340 if y<0 then y=y+ysize
350 if y>=ysize then y=y-ysize
360 gosub 440
370 if x>xsize/2 then fx=-xsize else fx=xsize
380 if y>ysize/2 then fy=-ysize else fy=ysize
390 x=x+fx:gosub 440
400 y=y+fy:gosub 440
410 x=x-fx:gosub 440
420 y=y-fy
430 return
440 rem ************************   Draw ship
450 pen 2
460 as=2.5
470 r=20
480 r2=r*.8
490 color 255,255,255
500 move x+r*cos(a), y-r*sin(a)
510 line x+r2*cos(a+as), y-r2*sin(a+as)
520 line x+r2*cos(a-as), y-r2*sin(a-as)
530 line x+r*cos(a), y-r*sin(a)
540 if fire=0 then 620
550 if fc&1 = 0 then 620
560 r2=r*.7
570 move x-r2*cos(a), y+r2*sin(a)
580 r2=r2*2
590 color 255,190,0
600 pen 4
610 line x-r2*cos(a), y+r2*sin(a)
620 return
630 rem ************************  Hacks
640 code = keycode
650 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
660 if code=32 then gosub 910
670 return
680 rem ************************ Handle missiles
690 for i = 1 to missnum
700 missx(i) = missx(i) + missdx(i)
710 if missx(i)<0 then missx(i) = missx(i) + xsize
720 if missx(i)>=xsize then missx(i) = missx(i) - xsize
730 missy(i) = missy(i) + missdy(i)
740 if missy(i)<0 then missx(i) = missx(i) + ysize
750 if missy(i)>=ysize then missy(i) = missy(i) - ysize
760 color 255,255,255
770 disc missx(i), missy(i), 5
780 misst(i) = misst(i) - 1
790 next i
800 if misst(1) >= 0 then 900
810 missnum = nissnum - 1
820 if missnum = 0 then 900
830 for i = 1 to missnum
840 missx(i) = missx(i+1)
850 missy(i) = missy(i+1)
860 missdx(i) = missdx(i+1)
870 missdy(i) = missdy(i+1)
880 misst(i) = misst(i+1)
890 next i
900 return
910 rem ************************   Fire missile
920 missnum = missnum + 1
930 misst(missnum) = 50
940 missx(missnum) = x
950 missy(missnum) = y
960 mspeed = mv*1.2
970 missdx(missnum) = dx + mspeed*cos(a)
980 missdy(missnum) = dy - mspeed*sin(a)
990 return
