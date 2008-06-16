10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 dim ex(100), ey(100), edx(100), edy(100), et(100)
50 rem ****** restart here
60 restart = 0
70 dead = 0
80 wantm = 0
90 fc = 0
100 lastm = -20
110 dx = 0: dy = 0
120 emax = 100 : rem max explosion dots
130 for i=1 to emax:et(i) = 0: next i
140 missnum = 0
150 starnum = 60
160 for i=1 to starnum
170 sx(i) = rnd(xsize)
180 sy(i) = rnd(ysize)
190 next i
200 pi2=3.1415928*2.0
210 shipx = xsize/2 : rem x position of ship
220 shipy = ysize/2 : rem y position of ship
230 thrust=.2 : rem thrust
240 v=0: rem velocity
250 mv=7 : rem max velocity
260 da = .1: rem rotation speed
270 a=3.1415928/2
280 gosub 1410
290 rem ************************   Main loop
300 cls
310 print wt
320 fc=fc+1
330 gosub 1340: rem stars
340 gosub 460: rem handle ship
350 gosub 910: rem hacks
360 gosub 970: rem missiles
370 gosub 1220: rem handle firing missiles
380 gosub 1590: rem asteroids
390 gosub 1840: rem missiles hitting asteroids
400 gosub 2240: rem handle explosion dots
410 gosub 2530: rem check if ship destroyed
420 update
430 wt = sleep(.02)
440 if restart=1 then 50
450 goto 290
460 rem ************************   Handle ship movement
470 if dead<>0 then 710
480 fire=0
490 if key(402)=0 then 550
500 dx = dx + thrust*cos(a)
510 dy = dy - thrust*sin(a)
520 v = sqr(dx*dx + dy*dy)
530 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
540 fire = 1
550 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
560 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
570 shipx=shipx+dx
580 if shipx<0 then shipx=shipx+xsize
590 if shipx>= xsize then shipx=shipx-xsize
600 shipy=shipy+dy
610 if shipy<0 then shipy=shipy+ysize
620 if shipy>=ysize then shipy=shipy-ysize
630 x = shipx
640 y = shipy
650 gosub 720
660 if x>xsize/2 then fx=-xsize else fx=xsize
670 if y>ysize/2 then fy=-ysize else fy=ysize
680 x=x+fx:gosub 720
690 y=y+fy:gosub 720
700 x=x-fx:gosub 720
710 return
720 rem ************************   Draw ship
730 pen 2
740 as=2.5
750 r=20
760 r2=r*.8
770 color 255,255,255
780 move x+r*cos(a), y-r*sin(a)
790 line x+r2*cos(a+as), y-r2*sin(a+as)
800 line x+r2*cos(a-as), y-r2*sin(a-as)
810 line x+r*cos(a), y-r*sin(a)
820 if fire=0 then 900
830 if fc&1 = 0 then 900
840 r2=r*.7
850 move x-r2*cos(a), y+r2*sin(a)
860 r2=r2*2
870 color 255,190,0
880 pen 4
890 line x-r2*cos(a), y+r2*sin(a)
900 return
910 rem ************************  Hacks
920 code = keycode
930 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
940 if code=410 then wantm=1: rem want to fire a missile
950 if code=32 then restart=1
960 return
970 rem ************************ Handle missiles
980 if missnum = 0 then 1210
990 for i = 1 to missnum
1000 if misst(i) <= 0 then 1100
1010 missx(i) = missx(i) + missdx(i)
1020 if missx(i)<0 then missx(i) = missx(i) + xsize
1030 if missx(i)>=xsize then missx(i) = missx(i) - xsize
1040 missy(i) = missy(i) + missdy(i)
1050 if missy(i)<0 then missy(i) = missy(i) + ysize
1060 if missy(i)>=ysize then missy(i) = missy(i) - ysize
1070 color 255,255,255
1080 disc missx(i), missy(i), 2
1090 misst(i) = misst(i) - 1
1100 next i
1110 if misst(1) > 0 then 1210
1120 missnum = missnum - 1
1130 if missnum = 0 then 1210
1140 for i = 1 to missnum
1150 missx(i) = missx(i+1)
1160 missy(i) = missy(i+1)
1170 missdx(i) = missdx(i+1)
1180 missdy(i) = missdy(i+1)
1190 misst(i) = misst(i+1)
1200 next i
1210 return
1220 rem ************************   Fire missile
1230 if wantm = 0 then 1330
1240 if lastm+8 > fc then 1330
1250 lastm = fc
1260 missnum = missnum + 1
1270 misst(missnum) = 60
1280 missx(missnum) = x
1290 missy(missnum) = y
1300 mspeed = mv*1.2
1310 missdx(missnum) = dx + mspeed*cos(a)
1320 missdy(missnum) = dy - mspeed*sin(a)
1330 return
1340 rem ************************  Stars
1350 color 255,255,255
1360 wantm = 0
1370 for i=1 to starnum
1380 disc sx(i),sy(i),1
1390 next i
1400 return
1410 rem ************************ setup asteroids
1420 anum = 4
1430 amin = 15
1440 for i = 1 to anum
1450 tx = rnd(xsize)
1460 ty = rnd(ysize)
1470 tx2 = tx - shipx:ty2 = ty - shipy
1480 r=sqr(tx2*tx2 + ty2*ty2)
1490 if r < xsize/2 or r < ysize/2 then 1450
1500 rx(i) = tx
1510 ry(i) = ty
1520 ta = rnd(360) * 3.1415928/180.0
1530 r = 1: rem asteroid initial speed
1540 rdx(i) = r*cos(ta)
1550 rdy(i) = r*sin(ta)
1560 rs(i) = amin*4
1570 next i
1580 return
1590 rem ************************ handle asteroids
1600 if anum = 0 then 1810
1610 color 255,255,255
1620 for i = 1 to anum
1630 rx(i) = rx(i) + rdx(i)
1640 if rx(i) < 0 then rx(i) = rx(i) + xsize
1650 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1660 ry(i) = ry(i) + rdy(i)
1670 if ry(i) < 0 then ry(i) = ry(i) + ysize
1680 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1690 x = rx(i): y = ry(i) : s = rs(i)
1700 if x>xsize/2 then fx=-xsize else fx=xsize
1710 if y>ysize/2 then fy=-ysize else fy=ysize
1720 pen rs(i)*.2
1730 gosub 1820
1740 x=x+fx
1750 gosub 1820
1760 y=y+fy
1770 gosub 1820
1780 x=x-fx
1790 gosub 1820
1800 next i
1810 return
1820 circle x,y,s
1830 return
1840 rem *********************** handle missiles hitting asteroids
1850 if missnum = 0 or anum = 0 then 2230
1860 for i=1 to missnum
1870 x = missx(i):y=missy(i)
1880 if misst(i) <= 0 then 2130
1890 for j=1 to anum
1900 if rs(j) = 0 then 2120
1910 tx = rx(j) - x
1920 ty = ry(j) - y
1930 r = sqr(tx*tx + ty*ty)
1940 if r>rs(j) then 2120
1950 gosub 2370
1960 misst(i) = 0
1970 rs(j) = rs(j)*.5
1980 if rs(j) < amin then rs(j) = 0:goto 2120
1990 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
2000 s = s * 2
2010 tx = s * tx/r
2020 ty = s * ty/r
2030 rdx(j) = -ty
2040 rdy(j) = tx
2050 anum = anum+1
2060 rx(anum) = rx(j)
2070 ry(anum) = ry(j)
2080 rdx(anum) = -rdx(j)
2090 rdy(anum) = -rdy(j)
2100 rs(anum) = rs(j)
2110 goto 2130
2120 next j
2130 next i
2140 i=1: goto 2220
2150 if rs(i) > 0 then i=i+1: goto 2220
2160 rx(i) = rx(anum)
2170 ry(i) = ry(anum)
2180 rdx(i) = rdx(anum)
2190 rdy(i) = rdy(anum)
2200 rs(i) = rs(anum)
2210 anum = anum - 1
2220 if i<= anum then 2150
2230 return
2240 rem ************************* handle explosion dots
2250 for i=1 to emax
2260 if et(i) <= 0 then 2350
2270 ex(i) = ex(i) + edx(i)
2280 if ex(i)<0 then ex(i) = ex(i) + xsize
2290 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2300 ey(i) = ey(i) + edy(i)
2310 if ey(i)<0 then ey(i) = ey(i) + ysize
2320 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2330 et(i) = et(i) - 1
2340 disc ex(i), ey(i), 1.5
2350 next i
2360 return
2370 rem *********************** Add some explosion dots
2380 rem x, y is coord
2390 uc=15
2400 for u = 1 to emax
2410 if et(u) >0 then 2510
2420 et(u) = 50 + rnd(10)
2430 ex(u) = x
2440 ey(u) = y
2450 ur = rnd(0)*2 + 1
2460 ut = rnd(360)*3.1415928/180
2470 edx(u) = ur*cos(ut)
2480 edy(u) = ur*sin(ut)
2490 uc = uc - 1
2500 if uc = 0 then 2520
2510 next u
2520 return
2530 rem ********************* Check if ship destroyed
2540 if dead<>0 then 2660
2550 if anum = 0 then 2660
2560 for i = 1 to anum
2570 tx = shipx - rx(i)
2580 ty = shipy - ry(i)
2590 r = sqr(tx*tx+ty*ty)
2600 if r > rs(i) then 2650
2610 dead=1
2620 x=shipx
2630 y=shipy
2640 gosub 2370: gosub 2370: gosub 2370
2650 next i
2660 return
