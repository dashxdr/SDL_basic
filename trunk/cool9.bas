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
190 print wt
200 fc=fc+1
210 gosub 1090: rem stars
220 gosub 280: rem handle ship
230 gosub 710: rem hacks
240 gosub 760: rem missiles
250 update
260 wt = sleep(.02)
270 goto 170
280 rem ************************   Handle ship movement
290 fire=0
300 if key(402)=0 then 360
310 dx = dx + thrust*cos(a)
320 dy = dy - thrust*sin(a)
330 v = sqr(dx*dx + dy*dy)
340 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
350 fire = 1
360 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
370 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
380 x=x+dx
390 if x<0 then x=x+xsize
400 if x>= xsize then x=x-xsize
410 y=y+dy
420 if y<0 then y=y+ysize
430 if y>=ysize then y=y-ysize
440 gosub 520
450 if x>xsize/2 then fx=-xsize else fx=xsize
460 if y>ysize/2 then fy=-ysize else fy=ysize
470 x=x+fx:gosub 520
480 y=y+fy:gosub 520
490 x=x-fx:gosub 520
500 y=y-fy
510 return
520 rem ************************   Draw ship
530 pen 2
540 as=2.5
550 r=20
560 r2=r*.8
570 color 255,255,255
580 move x+r*cos(a), y-r*sin(a)
590 line x+r2*cos(a+as), y-r2*sin(a+as)
600 line x+r2*cos(a-as), y-r2*sin(a-as)
610 line x+r*cos(a), y-r*sin(a)
620 if fire=0 then 700
630 if fc&1 = 0 then 700
640 r2=r*.7
650 move x-r2*cos(a), y+r2*sin(a)
660 r2=r2*2
670 color 255,190,0
680 pen 4
690 line x-r2*cos(a), y+r2*sin(a)
700 return
710 rem ************************  Hacks
720 code = keycode
730 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
740 if code=410 then gosub 1000
750 return
760 rem ************************ Handle missiles
770 if missnum = 0 then 990
780 for i = 1 to missnum
790 missx(i) = missx(i) + missdx(i)
800 if missx(i)<0 then missx(i) = missx(i) + xsize
810 if missx(i)>=xsize then missx(i) = missx(i) - xsize
820 missy(i) = missy(i) + missdy(i)
830 if missy(i)<0 then missy(i) = missy(i) + ysize
840 if missy(i)>=ysize then missy(i) = missy(i) - ysize
850 color 255,255,255
860 disc missx(i), missy(i), 2
870 misst(i) = misst(i) - 1
880 next i
890 if misst(1) >= 0 then 990
900 missnum = missnum - 1
910 if missnum = 0 then 990
920 for i = 1 to missnum
930 missx(i) = missx(i+1)
940 missy(i) = missy(i+1)
950 missdx(i) = missdx(i+1)
960 missdy(i) = missdy(i+1)
970 misst(i) = misst(i+1)
980 next i
990 return
1000 rem ************************   Fire missile
1010 missnum = missnum + 1
1020 misst(missnum) = 80
1030 missx(missnum) = x
1040 missy(missnum) = y
1050 mspeed = mv*1.2
1060 missdx(missnum) = dx + mspeed*cos(a)
1070 missdy(missnum) = dy - mspeed*sin(a)
1080 return
1090 rem ************************  Stars
1100 color 255,255,255
1110 for i=1 to starnum
1120 disc sx(i),sy(i),1
1130 next i
1140 return
