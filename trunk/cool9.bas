10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 missnum = 0
40 starnum = 60
50 for i=1 to starnum
60 sx(i) = rnd(xsize)
70 sy(i) = rnd(ysize)
80 next i
90 pi2=3.1415928*2.0
100 x = xsize/2 : rem x position of ship
110 y = ysize/2 : rem y position of ship
120 thrust=.2 : rem thrust
130 v=0: rem velocity
140 mv=7 : rem max velocity
150 da = .1: rem rotation speed
160 a=3.1415928/2
170 rem ************************   Main loop
180 cls
190 print sleep(.02);
200 fc=fc+1
210 gosub 1080: rem stars
220 gosub 270: rem handle ship
230 gosub 700: rem hacks
240 gosub 750: rem missiles
250 update
260 goto 170
270 rem ************************   Handle ship movement
280 fire=0
290 if key(402)=0 then 350
300 dx = dx + thrust*cos(a)
310 dy = dy - thrust*sin(a)
320 v = sqr(dx*dx + dy*dy)
330 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
340 fire = 1
350 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
360 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
370 x=x+dx
380 if x<0 then x=x+xsize
390 if x>= xsize then x=x-xsize
400 y=y+dy
410 if y<0 then y=y+ysize
420 if y>=ysize then y=y-ysize
430 gosub 510
440 if x>xsize/2 then fx=-xsize else fx=xsize
450 if y>ysize/2 then fy=-ysize else fy=ysize
460 x=x+fx:gosub 510
470 y=y+fy:gosub 510
480 x=x-fx:gosub 510
490 y=y-fy
500 return
510 rem ************************   Draw ship
520 pen 2
530 as=2.5
540 r=20
550 r2=r*.8
560 color 255,255,255
570 move x+r*cos(a), y-r*sin(a)
580 line x+r2*cos(a+as), y-r2*sin(a+as)
590 line x+r2*cos(a-as), y-r2*sin(a-as)
600 line x+r*cos(a), y-r*sin(a)
610 if fire=0 then 690
620 if fc&1 = 0 then 690
630 r2=r*.7
640 move x-r2*cos(a), y+r2*sin(a)
650 r2=r2*2
660 color 255,190,0
670 pen 4
680 line x-r2*cos(a), y+r2*sin(a)
690 return
700 rem ************************  Hacks
710 code = keycode
720 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
730 if code=410 then gosub 990
740 return
750 rem ************************ Handle missiles
760 if missnum = 0 then 980
770 for i = 1 to missnum
780 missx(i) = missx(i) + missdx(i)
790 if missx(i)<0 then missx(i) = missx(i) + xsize
800 if missx(i)>=xsize then missx(i) = missx(i) - xsize
810 missy(i) = missy(i) + missdy(i)
820 if missy(i)<0 then missy(i) = missy(i) + ysize
830 if missy(i)>=ysize then missy(i) = missy(i) - ysize
840 color 255,255,255
850 disc missx(i), missy(i), 2
860 misst(i) = misst(i) - 1
870 next i
880 if misst(1) >= 0 then 980
890 missnum = missnum - 1
900 if missnum = 0 then 980
910 for i = 1 to missnum
920 missx(i) = missx(i+1)
930 missy(i) = missy(i+1)
940 missdx(i) = missdx(i+1)
950 missdy(i) = missdy(i+1)
960 misst(i) = misst(i+1)
970 next i
980 return
990 rem ************************   Fire missile
1000 missnum = missnum + 1
1010 misst(missnum) = 80
1020 missx(missnum) = x
1030 missy(missnum) = y
1040 mspeed = mv*1.2
1050 missdx(missnum) = dx + mspeed*cos(a)
1060 missdy(missnum) = dy - mspeed*sin(a)
1070 return
1080 rem ************************  Stars
1090 color 255,255,255
1100 for i=1 to starnum
1110 disc sx(i),sy(i),1
1120 next i
1130 return
