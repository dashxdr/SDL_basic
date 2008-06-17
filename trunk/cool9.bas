10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 dim ex(100), ey(100), edx(100), edy(100), et(100)
50 rem ****** restart here
60 restart = 0
70 timer=0
80 dead = 0
90 wantm = 0
100 fc = 0
110 lastm = -20
120 dx = 0: dy = 0
130 emax = 100 : rem max explosion dots
140 for i=1 to emax:et(i) = 0: next i
150 missnum = 0
160 starnum = 60
170 for i=1 to starnum
180 sx(i) = rnd(xsize)
190 sy(i) = rnd(ysize)
200 next i
210 pi2=3.1415928*2.0
220 shipx = xsize/2 : rem x position of ship
230 shipy = ysize/2 : rem y position of ship
240 thrust=.2 : rem thrust
250 v=0: rem velocity
260 mv=7 : rem max velocity
270 da = .1: rem rotation speed
280 shipa=3.1415928/2: rem Angle ship is facing
290 gosub 1430
300 rem ************************   Main loop
310 cls
320 print wt
330 fc=fc+1
340 gosub 1360: rem stars
350 gosub 480: rem handle ship
360 gosub 930: rem hacks
370 gosub 990: rem missiles
380 gosub 1240: rem handle firing missiles
390 gosub 1610: rem asteroids
400 gosub 1860: rem missiles hitting asteroids
410 gosub 2260: rem handle explosion dots
420 gosub 2550: rem check if ship destroyed
430 gosub 2690: rem housekeeping
440 update
450 wt = sleep(.02)
460 if restart=1 then 50
470 goto 300
480 rem ************************   Handle ship movement
490 if dead<>0 then 730
500 fire=0
510 if key(402)=0 then 570
520 dx = dx + thrust*cos(a)
530 dy = dy - thrust*sin(a)
540 v = sqr(dx*dx + dy*dy)
550 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
560 fire = 1
570 if key(400) then shipa=shipa+da:if(shipa<0.0) then shipa=shipa+pi2
580 if key(401) then shipa=shipa-da:if(shipa>pi2) then shipa=shipa-pi2
590 shipx=shipx+dx
600 if shipx<0 then shipx=shipx+xsize
610 if shipx>= xsize then shipx=shipx-xsize
620 shipy=shipy+dy
630 if shipy<0 then shipy=shipy+ysize
640 if shipy>=ysize then shipy=shipy-ysize
645 a = shipa
650 x = shipx
660 y = shipy
670 gosub 740
680 if x>xsize/2 then fx=-xsize else fx=xsize
690 if y>ysize/2 then fy=-ysize else fy=ysize
700 x=x+fx:gosub 740
710 y=y+fy:gosub 740
720 x=x-fx:gosub 740
730 return
740 rem ************************   Draw ship
750 pen 2
760 as=2.5
770 r=20
780 r2=r*.8
790 color 255,255,255
800 move x+r*cos(a), y-r*sin(a)
810 line x+r2*cos(a+as), y-r2*sin(a+as)
820 line x+r2*cos(a-as), y-r2*sin(a-as)
830 line x+r*cos(a), y-r*sin(a)
840 if fire=0 then 920
850 if fc&1 = 0 then 920
860 r2=r*.7
870 move x-r2*cos(a), y+r2*sin(a)
880 r2=r2*2
890 color 255,190,0
900 pen 4
910 line x-r2*cos(a), y+r2*sin(a)
920 return
930 rem ************************  Hacks
940 code = keycode
950 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
960 if code=410 then wantm=1: rem want to fire a missile
970 if code=32 then restart=1
980 return
990 rem ************************ Handle missiles
1000 if missnum = 0 then 1230
1010 for i = 1 to missnum
1020 if misst(i) <= 0 then 1120
1030 missx(i) = missx(i) + missdx(i)
1040 if missx(i)<0 then missx(i) = missx(i) + xsize
1050 if missx(i)>=xsize then missx(i) = missx(i) - xsize
1060 missy(i) = missy(i) + missdy(i)
1070 if missy(i)<0 then missy(i) = missy(i) + ysize
1080 if missy(i)>=ysize then missy(i) = missy(i) - ysize
1090 color 255,255,255
1100 disc missx(i), missy(i), 2
1110 misst(i) = misst(i) - 1
1120 next i
1130 if misst(1) > 0 then 1230
1140 missnum = missnum - 1
1150 if missnum = 0 then 1230
1160 for i = 1 to missnum
1170 missx(i) = missx(i+1)
1180 missy(i) = missy(i+1)
1190 missdx(i) = missdx(i+1)
1200 missdy(i) = missdy(i+1)
1210 misst(i) = misst(i+1)
1220 next i
1230 return
1240 rem ************************   Fire missile
1250 if wantm = 0 then 1350
1260 if lastm+8 > fc then 1350
1270 lastm = fc
1280 missnum = missnum + 1
1290 misst(missnum) = 60
1300 missx(missnum) = x
1310 missy(missnum) = y
1320 mspeed = mv*1.2
1330 missdx(missnum) = dx + mspeed*cos(shopa)
1340 missdy(missnum) = dy - mspeed*sin(shipa)
1350 return
1360 rem ************************  Stars
1370 color 255,255,255
1380 wantm = 0
1390 for i=1 to starnum
1400 disc sx(i),sy(i),1
1410 next i
1420 return
1430 rem ************************ setup asteroids
1440 anum = 4
1450 amin = 15
1460 for i = 1 to anum
1470 tx = rnd(xsize)
1480 ty = rnd(ysize)
1490 tx2 = tx - shipx:ty2 = ty - shipy
1500 r=sqr(tx2*tx2 + ty2*ty2)
1510 if r < xsize/2 or r < ysize/2 then 1470
1520 rx(i) = tx
1530 ry(i) = ty
1540 ta = rnd(360) * 3.1415928/180.0
1550 r = 1: rem asteroid initial speed
1560 rdx(i) = r*cos(ta)
1570 rdy(i) = r*sin(ta)
1580 rs(i) = amin*4
1590 next i
1600 return
1610 rem ************************ handle asteroids
1620 if anum = 0 then 1830
1630 color 255,255,255
1640 for i = 1 to anum
1650 rx(i) = rx(i) + rdx(i)
1660 if rx(i) < 0 then rx(i) = rx(i) + xsize
1670 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1680 ry(i) = ry(i) + rdy(i)
1690 if ry(i) < 0 then ry(i) = ry(i) + ysize
1700 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1710 x = rx(i): y = ry(i) : s = rs(i)
1720 if x>xsize/2 then fx=-xsize else fx=xsize
1730 if y>ysize/2 then fy=-ysize else fy=ysize
1740 pen rs(i)*.2
1750 gosub 1840
1760 x=x+fx
1770 gosub 1840
1780 y=y+fy
1790 gosub 1840
1800 x=x-fx
1810 gosub 1840
1820 next i
1830 return
1840 circle x,y,s
1850 return
1860 rem *********************** handle missiles hitting asteroids
1870 if missnum = 0 or anum = 0 then 2250
1880 for i=1 to missnum
1890 x = missx(i):y=missy(i)
1900 if misst(i) <= 0 then 2150
1910 for j=1 to anum
1920 if rs(j) = 0 then 2140
1930 tx = rx(j) - x
1940 ty = ry(j) - y
1950 r = sqr(tx*tx + ty*ty)
1960 if r>rs(j) then 2140
1970 gosub 2390
1980 misst(i) = 0
1990 rs(j) = rs(j)*.5
2000 if rs(j) < amin then rs(j) = 0:goto 2140
2010 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
2020 s = s * 2
2030 tx = s * tx/r
2040 ty = s * ty/r
2050 rdx(j) = -ty
2060 rdy(j) = tx
2070 anum = anum+1
2080 rx(anum) = rx(j)
2090 ry(anum) = ry(j)
2100 rdx(anum) = -rdx(j)
2110 rdy(anum) = -rdy(j)
2120 rs(anum) = rs(j)
2130 goto 2150
2140 next j
2150 next i
2160 i=1: goto 2240
2170 if rs(i) > 0 then i=i+1: goto 2240
2180 rx(i) = rx(anum)
2190 ry(i) = ry(anum)
2200 rdx(i) = rdx(anum)
2210 rdy(i) = rdy(anum)
2220 rs(i) = rs(anum)
2230 anum = anum - 1
2240 if i<= anum then 2170
2250 return
2260 rem ************************* handle explosion dots
2270 for i=1 to emax
2280 if et(i) <= 0 then 2370
2290 ex(i) = ex(i) + edx(i)
2300 if ex(i)<0 then ex(i) = ex(i) + xsize
2310 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2320 ey(i) = ey(i) + edy(i)
2330 if ey(i)<0 then ey(i) = ey(i) + ysize
2340 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2350 et(i) = et(i) - 1
2360 disc ex(i), ey(i), 1.5
2370 next i
2380 return
2390 rem *********************** Add some explosion dots
2400 rem x, y is coord
2410 uc=15
2420 for u = 1 to emax
2430 if et(u) >0 then 2530
2440 et(u) = 50 + rnd(10)
2450 ex(u) = x
2460 ey(u) = y
2470 ur = rnd(0)*2 + 1
2480 ut = rnd(360)*3.1415928/180
2490 edx(u) = ur*cos(ut)
2500 edy(u) = ur*sin(ut)
2510 uc = uc - 1
2520 if uc = 0 then 2540
2530 next u
2540 return
2550 rem ********************* Check if ship destroyed
2560 if dead<>0 then 2680
2570 if anum = 0 then 2680
2580 for i = 1 to anum
2590 tx = shipx - rx(i)
2600 ty = shipy - ry(i)
2610 r = sqr(tx*tx+ty*ty)
2620 if r > rs(i) then 2670
2630 dead=1
2640 x=shipx
2650 y=shipy
2660 gosub 2390: gosub 2390: gosub 2390
2670 next i
2680 return
2690 rem ********************** Housekeeping functions
2700 if dead=0 then 2820
2710 timer = timer + 1
2720 if timer <150 then 2850
2730 shipx = xsize/2: shipy = ysize/2: dx=0: dy=0: a=3.1415928/2
2740 if anum = 0 then 2810
2750 for i=1 to anum
2760 tx = rx(i) - shipx
2770 ty = ry(i) - shipy
2780 r=sqr(tx*tx + ty*ty)
2790 if r<xsize/4 or r<ysize/4 then 2820
2800 next i
2810 dead = 0:timer=0
2820 if anum<>0 then 2850
2830 timer = timer + 1
2840 if timer = 150 then timer = 0:gosub 1430
2850 return
