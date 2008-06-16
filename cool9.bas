10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 dim ex(100), ey(100), edx(100), edy(100), et(100)
50 rem ****** restart here
60 dead = 0
70 dx = 0: dy = 0
80 emax = 100 : rem max explosion dots
90 for i=1 to emax:et(i) = 0: next i
100 missnum = 0
110 starnum = 60
120 for i=1 to starnum
130 sx(i) = rnd(xsize)
140 sy(i) = rnd(ysize)
150 next i
160 pi2=3.1415928*2.0
170 shipx = xsize/2 : rem x position of ship
180 shipy = ysize/2 : rem y position of ship
190 thrust=.2 : rem thrust
200 v=0: rem velocity
210 mv=7 : rem max velocity
220 da = .1: rem rotation speed
230 a=3.1415928/2
240 gosub 1320
250 rem ************************   Main loop
260 cls
270 print wt
280 print "missnum =";missnum
290 fc=fc+1
300 gosub 1260: rem stars
310 gosub 410: rem handle ship
320 gosub 860: rem hacks
330 gosub 920: rem missiles
340 gosub 1500: rem asteroids
350 gosub 1750: rem missiles hitting asteroids
360 gosub 2150: rem handle explosion dots
370 gosub 2440: rem check if ship destroyed
380 update
390 wt = sleep(.02)
400 goto 250
410 rem ************************   Handle ship movement
420 if dead<>0 then 660
430 fire=0
440 if key(402)=0 then 500
450 dx = dx + thrust*cos(a)
460 dy = dy - thrust*sin(a)
470 v = sqr(dx*dx + dy*dy)
480 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
490 fire = 1
500 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
510 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
520 shipx=shipx+dx
530 if shipx<0 then shipx=shipx+xsize
540 if shipx>= xsize then shipx=shipx-xsize
550 shipy=shipy+dy
560 if shipy<0 then shipy=shipy+ysize
570 if shipy>=ysize then shipy=shipy-ysize
580 x = shipx
590 y = shipy
600 gosub 670
610 if x>xsize/2 then fx=-xsize else fx=xsize
620 if y>ysize/2 then fy=-ysize else fy=ysize
630 x=x+fx:gosub 670
640 y=y+fy:gosub 670
650 x=x-fx:gosub 670
660 return
670 rem ************************   Draw ship
680 pen 2
690 as=2.5
700 r=20
710 r2=r*.8
720 color 255,255,255
730 move x+r*cos(a), y-r*sin(a)
740 line x+r2*cos(a+as), y-r2*sin(a+as)
750 line x+r2*cos(a-as), y-r2*sin(a-as)
760 line x+r*cos(a), y-r*sin(a)
770 if fire=0 then 850
780 if fc&1 = 0 then 850
790 r2=r*.7
800 move x-r2*cos(a), y+r2*sin(a)
810 r2=r2*2
820 color 255,190,0
830 pen 4
840 line x-r2*cos(a), y+r2*sin(a)
850 return
860 rem ************************  Hacks
870 code = keycode
880 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
890 if code=410 then gosub 1170
900 if code=32 then 50
910 return
920 rem ************************ Handle missiles
930 if missnum = 0 then 1160
940 for i = 1 to missnum
950 if misst(i) <= 0 then 1050
960 missx(i) = missx(i) + missdx(i)
970 if missx(i)<0 then missx(i) = missx(i) + xsize
980 if missx(i)>=xsize then missx(i) = missx(i) - xsize
990 missy(i) = missy(i) + missdy(i)
1000 if missy(i)<0 then missy(i) = missy(i) + ysize
1010 if missy(i)>=ysize then missy(i) = missy(i) - ysize
1020 color 255,255,255
1030 disc missx(i), missy(i), 2
1040 misst(i) = misst(i) - 1
1050 next i
1060 if misst(1) > 0 then 1160
1070 missnum = missnum - 1
1080 if missnum = 0 then 1160
1090 for i = 1 to missnum
1100 missx(i) = missx(i+1)
1110 missy(i) = missy(i+1)
1120 missdx(i) = missdx(i+1)
1130 missdy(i) = missdy(i+1)
1140 misst(i) = misst(i+1)
1150 next i
1160 return
1170 rem ************************   Fire missile
1180 missnum = missnum + 1
1190 misst(missnum) = 80
1200 missx(missnum) = x
1210 missy(missnum) = y
1220 mspeed = mv*1.2
1230 missdx(missnum) = dx + mspeed*cos(a)
1240 missdy(missnum) = dy - mspeed*sin(a)
1250 return
1260 rem ************************  Stars
1270 color 255,255,255
1280 for i=1 to starnum
1290 disc sx(i),sy(i),1
1300 next i
1310 return
1320 rem ************************ setup asteroids
1330 anum = 6
1340 amin = 15
1350 for i = 1 to anum
1360 tx = rnd(xsize)
1370 ty = rnd(ysize)
1380 tx2 = tx - shipx:ty2 = ty - shipy
1390 r=sqr(tx2*tx2 + ty2*ty2)
1400 if r < xsize/2 or r < ysize/2 then 1360
1410 rx(i) = tx
1420 ry(i) = ty
1430 ta = rnd(360) * 3.1415928/180.0
1440 r = 1: rem asteroid initial speed
1450 rdx(i) = r*cos(ta)
1460 rdy(i) = r*sin(ta)
1470 rs(i) = amin*4
1480 next i
1490 return
1500 rem ************************ handle asteroids
1510 if anum = 0 then 1720
1520 color 255,255,255
1530 for i = 1 to anum
1540 rx(i) = rx(i) + rdx(i)
1550 if rx(i) < 0 then rx(i) = rx(i) + xsize
1560 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1570 ry(i) = ry(i) + rdy(i)
1580 if ry(i) < 0 then ry(i) = ry(i) + ysize
1590 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1600 x = rx(i): y = ry(i) : s = rs(i)
1610 if x>xsize/2 then fx=-xsize else fx=xsize
1620 if y>ysize/2 then fy=-ysize else fy=ysize
1630 pen rs(i)*.2
1640 gosub 1730
1650 x=x+fx
1660 gosub 1730
1670 y=y+fy
1680 gosub 1730
1690 x=x-fx
1700 gosub 1730
1710 next i
1720 return
1730 circle x,y,s
1740 return
1750 rem *********************** handle missiles hitting asteroids
1760 if missnum = 0 or anum = 0 then 2140
1770 for i=1 to missnum
1780 x = missx(i):y=missy(i)
1790 if misst(i) <= 0 then 2040
1800 for j=1 to anum
1810 if rs(j) = 0 then 2030
1820 tx = rx(j) - x
1830 ty = ry(j) - y
1840 r = sqr(tx*tx + ty*ty)
1850 if r>rs(j) then 2030
1860 gosub 2280
1870 rs(j) = rs(j)*.5
1880 if rs(j) < amin then rs(j) = 0:goto 2030
1890 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
1900 s = s * 2
1910 tx = s * tx/r
1920 ty = s * ty/r
1930 misst(i) = 0
1940 rdx(j) = -ty
1950 rdy(j) = tx
1960 anum = anum+1
1970 rx(anum) = rx(j)
1980 ry(anum) = ry(j)
1990 rdx(anum) = -rdx(j)
2000 rdy(anum) = -rdy(j)
2010 rs(anum) = rs(j)
2020 goto 2040
2030 next j
2040 next i
2050 i=1: goto 2130
2060 if rs(i) > 0 then i=i+1: goto 2130
2070 rx(i) = rx(anum)
2080 ry(i) = ry(anum)
2090 rdx(i) = rdx(anum)
2100 rdy(i) = rdy(anum)
2110 rs(i) = rs(anum)
2120 anum = anum - 1
2130 if i<= anum then 2060
2140 return
2150 rem ************************* handle explosion dots
2160 for i=1 to emax
2170 if et(i) <= 0 then 2260
2180 ex(i) = ex(i) + edx(i)
2190 if ex(i)<0 then ex(i) = ex(i) + xsize
2200 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2210 ey(i) = ey(i) + edy(i)
2220 if ey(i)<0 then ey(i) = ey(i) + ysize
2230 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2240 et(i) = et(i) - 1
2250 disc ex(i), ey(i), 1.5
2260 next i
2270 return
2280 rem *********************** Add some explosion dots
2290 rem x, y is coord
2300 uc=15
2310 for u = 1 to emax
2320 if et(u) >0 then 2420
2330 et(u) = 50
2340 ex(u) = x
2350 ey(u) = y
2360 ur = rnd(0)*2 + 1
2370 ut = rnd(360)*3.1415928/180
2380 edx(u) = ur*cos(ut)
2390 edy(u) = ur*sin(ut)
2400 uc = uc - 1
2410 if uc = 0 then 2430
2420 next u
2430 return
2440 rem ********************* Check if ship destroyed
2450 if dead<>0 then 2570
2460 if anum = 0 then 2570
2470 for i = 1 to anum
2480 tx = shipx - rx(i)
2490 ty = shipy - ry(i)
2500 r = sqr(tx*tx+ty*ty)
2510 if r > rs(i) then 2560
2520 dead=1
2530 x=shipx
2540 y=shipy
2550 gosub 2280: gosub 2280: gosub 2280
2560 next i
2570 return
