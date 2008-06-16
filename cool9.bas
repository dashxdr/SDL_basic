10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 dim ex(100), ey(100), edx(100), edy(100), et(100)
50 emax = 100 : rem max explosion dots
60 missnum = 0
70 starnum = 60
80 for i=1 to starnum
90 sx(i) = rnd(xsize)
100 sy(i) = rnd(ysize)
110 next i
120 pi2=3.1415928*2.0
130 shipx = xsize/2 : rem x position of ship
140 shipy = ysize/2 : rem y position of ship
150 thrust=.2 : rem thrust
160 v=0: rem velocity
170 mv=7 : rem max velocity
180 da = .1: rem rotation speed
190 a=3.1415928/2
200 gosub 1270
210 rem ************************   Main loop
220 cls
230 print wt
240 print "missnum =";missnum
250 fc=fc+1
260 gosub 1210: rem stars
270 gosub 370: rem handle ship
280 gosub 820: rem hacks
290 gosub 870: rem missiles
300 gosub 1450: rem asteroids
310 gosub 1700: rem missiles hitting asteroids
320 gosub 2100: rem handle explosion dots
330 gosub 2390: rem check if ship destroyed
340 update
350 wt = sleep(.02)
360 goto 210
370 rem ************************   Handle ship movement
380 if dead<>0 then 620
390 fire=0
400 if key(402)=0 then 460
410 dx = dx + thrust*cos(a)
420 dy = dy - thrust*sin(a)
430 v = sqr(dx*dx + dy*dy)
440 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
450 fire = 1
460 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
470 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
480 shipx=shipx+dx
490 if shipx<0 then shipx=shipx+xsize
500 if shipx>= xsize then shipx=shipx-xsize
510 shipy=shipy+dy
520 if shipy<0 then shipy=shipy+ysize
530 if shipy>=ysize then shipy=shipy-ysize
540 x = shipx
550 y = shipy
560 gosub 630
570 if x>xsize/2 then fx=-xsize else fx=xsize
580 if y>ysize/2 then fy=-ysize else fy=ysize
590 x=x+fx:gosub 630
600 y=y+fy:gosub 630
610 x=x-fx:gosub 630
620 return
630 rem ************************   Draw ship
640 pen 2
650 as=2.5
660 r=20
670 r2=r*.8
680 color 255,255,255
690 move x+r*cos(a), y-r*sin(a)
700 line x+r2*cos(a+as), y-r2*sin(a+as)
710 line x+r2*cos(a-as), y-r2*sin(a-as)
720 line x+r*cos(a), y-r*sin(a)
730 if fire=0 then 810
740 if fc&1 = 0 then 810
750 r2=r*.7
760 move x-r2*cos(a), y+r2*sin(a)
770 r2=r2*2
780 color 255,190,0
790 pen 4
800 line x-r2*cos(a), y+r2*sin(a)
810 return
820 rem ************************  Hacks
830 code = keycode
840 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
850 if code=410 then gosub 1120
860 return
870 rem ************************ Handle missiles
880 if missnum = 0 then 1110
890 for i = 1 to missnum
900 if misst(i) <= 0 then 1000
910 missx(i) = missx(i) + missdx(i)
920 if missx(i)<0 then missx(i) = missx(i) + xsize
930 if missx(i)>=xsize then missx(i) = missx(i) - xsize
940 missy(i) = missy(i) + missdy(i)
950 if missy(i)<0 then missy(i) = missy(i) + ysize
960 if missy(i)>=ysize then missy(i) = missy(i) - ysize
970 color 255,255,255
980 disc missx(i), missy(i), 2
990 misst(i) = misst(i) - 1
1000 next i
1010 if misst(1) > 0 then 1110
1020 missnum = missnum - 1
1030 if missnum = 0 then 1110
1040 for i = 1 to missnum
1050 missx(i) = missx(i+1)
1060 missy(i) = missy(i+1)
1070 missdx(i) = missdx(i+1)
1080 missdy(i) = missdy(i+1)
1090 misst(i) = misst(i+1)
1100 next i
1110 return
1120 rem ************************   Fire missile
1130 missnum = missnum + 1
1140 misst(missnum) = 80
1150 missx(missnum) = x
1160 missy(missnum) = y
1170 mspeed = mv*1.2
1180 missdx(missnum) = dx + mspeed*cos(a)
1190 missdy(missnum) = dy - mspeed*sin(a)
1200 return
1210 rem ************************  Stars
1220 color 255,255,255
1230 for i=1 to starnum
1240 disc sx(i),sy(i),1
1250 next i
1260 return
1270 rem ************************ setup asteroids
1280 anum = 6
1290 amin = 15
1300 for i = 1 to anum
1310 tx = rnd(xsize)
1320 ty = rnd(ysize)
1330 tx2 = tx - shipx:ty2 = ty - shipy
1340 r=sqr(tx2*tx2 + ty2*ty2)
1350 if r < xsize/2 or r < ysize/2 then 1310
1360 rx(i) = tx
1370 ry(i) = ty
1380 ta = rnd(360) * 3.1415928/180.0
1390 r = 1: rem asteroid initial speed
1400 rdx(i) = r*cos(ta)
1410 rdy(i) = r*sin(ta)
1420 rs(i) = amin*4
1430 next i
1440 return
1450 rem ************************ handle asteroids
1460 if anum = 0 then 1670
1470 color 255,255,255
1480 for i = 1 to anum
1490 rx(i) = rx(i) + rdx(i)
1500 if rx(i) < 0 then rx(i) = rx(i) + xsize
1510 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1520 ry(i) = ry(i) + rdy(i)
1530 if ry(i) < 0 then ry(i) = ry(i) + ysize
1540 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1550 x = rx(i): y = ry(i) : s = rs(i)
1560 if x>xsize/2 then fx=-xsize else fx=xsize
1570 if y>ysize/2 then fy=-ysize else fy=ysize
1580 pen rs(i)*.2
1590 gosub 1680
1600 x=x+fx
1610 gosub 1680
1620 y=y+fy
1630 gosub 1680
1640 x=x-fx
1650 gosub 1680
1660 next i
1670 return
1680 circle x,y,s
1690 return
1700 rem *********************** handle missiles hitting asteroids
1710 if missnum = 0 or anum = 0 then 2090
1720 for i=1 to missnum
1730 x = missx(i):y=missy(i)
1740 if misst(i) <= 0 then 1990
1750 for j=1 to anum
1760 if rs(j) = 0 then 1980
1770 tx = rx(j) - x
1780 ty = ry(j) - y
1790 r = sqr(tx*tx + ty*ty)
1800 if r>rs(j) then 1980
1810 gosub 2230
1820 rs(j) = rs(j)*.5
1830 if rs(j) < amin then rs(j) = 0:goto 1980
1840 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
1850 s = s * 2
1860 tx = s * tx/r
1870 ty = s * ty/r
1880 misst(i) = 0
1890 rdx(j) = -ty
1900 rdy(j) = tx
1910 anum = anum+1
1920 rx(anum) = rx(j)
1930 ry(anum) = ry(j)
1940 rdx(anum) = -rdx(j)
1950 rdy(anum) = -rdy(j)
1960 rs(anum) = rs(j)
1970 goto 1990
1980 next j
1990 next i
2000 i=1: goto 2080
2010 if rs(i) > 0 then i=i+1: goto 2080
2020 rx(i) = rx(anum)
2030 ry(i) = ry(anum)
2040 rdx(i) = rdx(anum)
2050 rdy(i) = rdy(anum)
2060 rs(i) = rs(anum)
2070 anum = anum - 1
2080 if i<= anum then 2010
2090 return
2100 rem ************************* handle explosion dots
2110 for i=1 to emax
2120 if et(i) <= 0 then 2210
2130 ex(i) = ex(i) + edx(i)
2140 if ex(i)<0 then ex(i) = ex(i) + xsize
2150 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2160 ey(i) = ey(i) + edy(i)
2170 if ey(i)<0 then ey(i) = ey(i) + ysize
2180 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2190 et(i) = et(i) - 1
2200 disc ex(i), ey(i), 1.5
2210 next i
2220 return
2230 rem *********************** Add some explosion dots
2240 rem x, y is coord
2250 uc=15
2260 for u = 1 to emax
2270 if et(u) >0 then 2370
2280 et(u) = 50
2290 ex(u) = x
2300 ey(u) = y
2310 ur = rnd(0)*2 + 1
2320 ut = rnd(360)*3.1415928/180
2330 edx(u) = ur*cos(ut)
2340 edy(u) = ur*sin(ut)
2350 uc = uc - 1
2360 if uc = 0 then 2380
2370 next u
2380 return
2390 rem ********************* Check if ship destroyed
2395 if dead<>0 then 2510
2400 if anum = 0 then 2510
2410 for i = 1 to anum
2420 tx = shipx - rx(i)
2430 ty = shipy - ry(i)
2440 r = sqr(tx*tx+ty*ty)
2450 if r > rs(i) then 2500
2460 dead=1
2470 x=shipx
2480 y=shipy
2490 gosub 2230: gosub 2230: gosub 2230
2500 next i
2510 return
