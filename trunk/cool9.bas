10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 missnum = 0
50 starnum = 60
60 for i=1 to starnum
70 sx(i) = rnd(xsize)
80 sy(i) = rnd(ysize)
90 next i
100 pi2=3.1415928*2.0
110 shipx = xsize/2 : rem x position of ship
120 shipy = ysize/2 : rem y position of ship
130 thrust=.2 : rem thrust
140 v=0: rem velocity
150 mv=7 : rem max velocity
160 da = .1: rem rotation speed
170 a=3.1415928/2
180 gosub 1220
190 rem ************************   Main loop
200 cls
210 print wt
220 print "missnum =";missnum
230 fc=fc+1
240 gosub 1160: rem stars
250 gosub 330: rem handle ship
260 gosub 770: rem hacks
270 gosub 820: rem missiles
280 gosub 1400: rem asteroids
290 gosub 1650: rem missiles hitting asteroids
300 update
310 wt = sleep(.02)
320 goto 190
330 rem ************************   Handle ship movement
340 fire=0
350 if key(402)=0 then 410
360 dx = dx + thrust*cos(a)
370 dy = dy - thrust*sin(a)
380 v = sqr(dx*dx + dy*dy)
390 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
400 fire = 1
410 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
420 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
430 shipx=shipx+dx
440 if shipx<0 then shipx=shipx+xsize
450 if shipx>= xsize then shipx=shipx-xsize
460 shipy=shipy+dy
470 if shipy<0 then shipy=shipy+ysize
480 if shipy>=ysize then shipy=shipy-ysize
490 x = shipx
500 y = shipy
510 gosub 580
520 if x>xsize/2 then fx=-xsize else fx=xsize
530 if y>ysize/2 then fy=-ysize else fy=ysize
540 x=x+fx:gosub 580
550 y=y+fy:gosub 580
560 x=x-fx:gosub 580
570 return
580 rem ************************   Draw ship
590 pen 2
600 as=2.5
610 r=20
620 r2=r*.8
630 color 255,255,255
640 move x+r*cos(a), y-r*sin(a)
650 line x+r2*cos(a+as), y-r2*sin(a+as)
660 line x+r2*cos(a-as), y-r2*sin(a-as)
670 line x+r*cos(a), y-r*sin(a)
680 if fire=0 then 760
690 if fc&1 = 0 then 760
700 r2=r*.7
710 move x-r2*cos(a), y+r2*sin(a)
720 r2=r2*2
730 color 255,190,0
740 pen 4
750 line x-r2*cos(a), y+r2*sin(a)
760 return
770 rem ************************  Hacks
780 code = keycode
790 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
800 if code=410 then gosub 1070
810 return
820 rem ************************ Handle missiles
830 if missnum = 0 then 1060
840 for i = 1 to missnum
850 if misst(i) <= 0 then 950
860 missx(i) = missx(i) + missdx(i)
870 if missx(i)<0 then missx(i) = missx(i) + xsize
880 if missx(i)>=xsize then missx(i) = missx(i) - xsize
890 missy(i) = missy(i) + missdy(i)
900 if missy(i)<0 then missy(i) = missy(i) + ysize
910 if missy(i)>=ysize then missy(i) = missy(i) - ysize
920 color 255,255,255
930 disc missx(i), missy(i), 2
940 misst(i) = misst(i) - 1
950 next i
960 if misst(1) > 0 then 1060
970 missnum = missnum - 1
980 if missnum = 0 then 1060
990 for i = 1 to missnum
1000 missx(i) = missx(i+1)
1010 missy(i) = missy(i+1)
1020 missdx(i) = missdx(i+1)
1030 missdy(i) = missdy(i+1)
1040 misst(i) = misst(i+1)
1050 next i
1060 return
1070 rem ************************   Fire missile
1080 missnum = missnum + 1
1090 misst(missnum) = 80
1100 missx(missnum) = x
1110 missy(missnum) = y
1120 mspeed = mv*1.2
1130 missdx(missnum) = dx + mspeed*cos(a)
1140 missdy(missnum) = dy - mspeed*sin(a)
1150 return
1160 rem ************************  Stars
1170 color 255,255,255
1180 for i=1 to starnum
1190 disc sx(i),sy(i),1
1200 next i
1210 return
1220 rem ************************ setup asteroids
1230 anum = 6
1240 amin = 15
1250 for i = 1 to anum
1260 tx = rnd(xsize)
1270 ty = rnd(ysize)
1280 tx2 = tx - shipx:ty2 = ty - shipy
1290 r=sqr(tx2*tx2 + ty2*ty2)
1300 if r < xsize/2 or r < ysize/2 then 1260
1310 rx(i) = tx
1320 ry(i) = ty
1330 ta = rnd(360) * 3.1415928/180.0
1340 r = 1: rem asteroid initial speed
1350 rdx(i) = r*cos(ta)
1360 rdy(i) = r*sin(ta)
1370 rs(i) = amin*4
1380 next i
1390 return
1400 rem ************************ handle asteroids
1410 if anum = 0 then 1620
1420 color 255,255,255
1430 for i = 1 to anum
1440 rx(i) = rx(i) + rdx(i)
1450 if rx(i) < 0 then rx(i) = rx(i) + xsize
1460 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1470 ry(i) = ry(i) + rdy(i)
1480 if ry(i) < 0 then ry(i) = ry(i) + ysize
1490 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1500 x = rx(i): y = ry(i) : s = rs(i)
1510 if x>xsize/2 then fx=-xsize else fx=xsize
1520 if y>ysize/2 then fy=-ysize else fy=ysize
1530 pen rs(i)*.2
1540 gosub 1630
1550 x=x+fx
1560 gosub 1630
1570 y=y+fy
1580 gosub 1630
1590 x=x-fx
1600 gosub 1630
1610 next i
1620 return
1630 circle x,y,s
1640 return
1650 rem *********************** handle missiles hitting asteroids
1660 if missnum = 0 or anum = 0 then 2030
1670 for i=1 to missnum
1680 x = missx(i):y=missy(i)
1690 if misst(i) <= 0 then 1930
1700 for j=1 to anum
1710 if rs(j) = 0 then 1920
1720 tx = rx(j) - x
1730 ty = ry(j) - y
1740 r = sqr(tx*tx + ty*ty)
1750 if r>rs(j) then 1920
1760 rs(j) = rs(j)*.5
1770 if rs(j) < amin then rs(j) = 0:goto 1920
1780 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
1790 s = s * 2
1800 tx = s * tx/r
1810 ty = s * ty/r
1820 misst(i) = 0
1830 rdx(j) = -ty
1840 rdy(j) = tx
1850 anum = anum+1
1860 rx(anum) = rx(j)
1870 ry(anum) = ry(j)
1880 rdx(anum) = -rdx(j)
1890 rdy(anum) = -rdy(j)
1900 rs(anum) = rs(j)
1910 goto 1930
1920 next j
1930 next i
1940 i=1: goto 2020
1950 if rs(i) > 0 then i=i+1: goto 2020
1960 rx(i) = rx(anum)
1970 ry(i) = ry(anum)
1980 rdx(i) = rdx(anum)
1990 rdy(i) = rdy(anum)
2000 rs(i) = rs(anum)
2010 anum = anum - 1
2020 if i<= anum then 1950
2030 return
