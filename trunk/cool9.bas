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
240 gosub 1310
250 rem ************************   Main loop
260 cls
270 print wt
280 fc=fc+1
290 gosub 1250: rem stars
300 gosub 400: rem handle ship
310 gosub 850: rem hacks
320 gosub 910: rem missiles
330 gosub 1490: rem asteroids
340 gosub 1740: rem missiles hitting asteroids
350 gosub 2140: rem handle explosion dots
360 gosub 2430: rem check if ship destroyed
370 update
380 wt = sleep(.02)
390 goto 250
400 rem ************************   Handle ship movement
410 if dead<>0 then 650
420 fire=0
430 if key(402)=0 then 490
440 dx = dx + thrust*cos(a)
450 dy = dy - thrust*sin(a)
460 v = sqr(dx*dx + dy*dy)
470 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
480 fire = 1
490 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
500 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
510 shipx=shipx+dx
520 if shipx<0 then shipx=shipx+xsize
530 if shipx>= xsize then shipx=shipx-xsize
540 shipy=shipy+dy
550 if shipy<0 then shipy=shipy+ysize
560 if shipy>=ysize then shipy=shipy-ysize
570 x = shipx
580 y = shipy
590 gosub 660
600 if x>xsize/2 then fx=-xsize else fx=xsize
610 if y>ysize/2 then fy=-ysize else fy=ysize
620 x=x+fx:gosub 660
630 y=y+fy:gosub 660
640 x=x-fx:gosub 660
650 return
660 rem ************************   Draw ship
670 pen 2
680 as=2.5
690 r=20
700 r2=r*.8
710 color 255,255,255
720 move x+r*cos(a), y-r*sin(a)
730 line x+r2*cos(a+as), y-r2*sin(a+as)
740 line x+r2*cos(a-as), y-r2*sin(a-as)
750 line x+r*cos(a), y-r*sin(a)
760 if fire=0 then 840
770 if fc&1 = 0 then 840
780 r2=r*.7
790 move x-r2*cos(a), y+r2*sin(a)
800 r2=r2*2
810 color 255,190,0
820 pen 4
830 line x-r2*cos(a), y+r2*sin(a)
840 return
850 rem ************************  Hacks
860 code = keycode
870 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
880 if code=410 then gosub 1160
890 if code=32 then 50
900 return
910 rem ************************ Handle missiles
920 if missnum = 0 then 1150
930 for i = 1 to missnum
940 if misst(i) <= 0 then 1040
950 missx(i) = missx(i) + missdx(i)
960 if missx(i)<0 then missx(i) = missx(i) + xsize
970 if missx(i)>=xsize then missx(i) = missx(i) - xsize
980 missy(i) = missy(i) + missdy(i)
990 if missy(i)<0 then missy(i) = missy(i) + ysize
1000 if missy(i)>=ysize then missy(i) = missy(i) - ysize
1010 color 255,255,255
1020 disc missx(i), missy(i), 2
1030 misst(i) = misst(i) - 1
1040 next i
1050 if misst(1) > 0 then 1150
1060 missnum = missnum - 1
1070 if missnum = 0 then 1150
1080 for i = 1 to missnum
1090 missx(i) = missx(i+1)
1100 missy(i) = missy(i+1)
1110 missdx(i) = missdx(i+1)
1120 missdy(i) = missdy(i+1)
1130 misst(i) = misst(i+1)
1140 next i
1150 return
1160 rem ************************   Fire missile
1170 missnum = missnum + 1
1180 misst(missnum) = 80
1190 missx(missnum) = x
1200 missy(missnum) = y
1210 mspeed = mv*1.2
1220 missdx(missnum) = dx + mspeed*cos(a)
1230 missdy(missnum) = dy - mspeed*sin(a)
1240 return
1250 rem ************************  Stars
1260 color 255,255,255
1270 for i=1 to starnum
1280 disc sx(i),sy(i),1
1290 next i
1300 return
1310 rem ************************ setup asteroids
1320 anum = 4
1330 amin = 15
1340 for i = 1 to anum
1350 tx = rnd(xsize)
1360 ty = rnd(ysize)
1370 tx2 = tx - shipx:ty2 = ty - shipy
1380 r=sqr(tx2*tx2 + ty2*ty2)
1390 if r < xsize/2 or r < ysize/2 then 1350
1400 rx(i) = tx
1410 ry(i) = ty
1420 ta = rnd(360) * 3.1415928/180.0
1430 r = 1: rem asteroid initial speed
1440 rdx(i) = r*cos(ta)
1450 rdy(i) = r*sin(ta)
1460 rs(i) = amin*4
1470 next i
1480 return
1490 rem ************************ handle asteroids
1500 if anum = 0 then 1710
1510 color 255,255,255
1520 for i = 1 to anum
1530 rx(i) = rx(i) + rdx(i)
1540 if rx(i) < 0 then rx(i) = rx(i) + xsize
1550 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1560 ry(i) = ry(i) + rdy(i)
1570 if ry(i) < 0 then ry(i) = ry(i) + ysize
1580 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1590 x = rx(i): y = ry(i) : s = rs(i)
1600 if x>xsize/2 then fx=-xsize else fx=xsize
1610 if y>ysize/2 then fy=-ysize else fy=ysize
1620 pen rs(i)*.2
1630 gosub 1720
1640 x=x+fx
1650 gosub 1720
1660 y=y+fy
1670 gosub 1720
1680 x=x-fx
1690 gosub 1720
1700 next i
1710 return
1720 circle x,y,s
1730 return
1740 rem *********************** handle missiles hitting asteroids
1750 if missnum = 0 or anum = 0 then 2130
1760 for i=1 to missnum
1770 x = missx(i):y=missy(i)
1780 if misst(i) <= 0 then 2030
1790 for j=1 to anum
1800 if rs(j) = 0 then 2020
1810 tx = rx(j) - x
1820 ty = ry(j) - y
1830 r = sqr(tx*tx + ty*ty)
1840 if r>rs(j) then 2020
1850 gosub 2270
1860 misst(i) = 0
1870 rs(j) = rs(j)*.5
1880 if rs(j) < amin then rs(j) = 0:goto 2020
1890 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
1900 s = s * 2
1910 tx = s * tx/r
1920 ty = s * ty/r
1930 rdx(j) = -ty
1940 rdy(j) = tx
1950 anum = anum+1
1960 rx(anum) = rx(j)
1970 ry(anum) = ry(j)
1980 rdx(anum) = -rdx(j)
1990 rdy(anum) = -rdy(j)
2000 rs(anum) = rs(j)
2010 goto 2030
2020 next j
2030 next i
2040 i=1: goto 2120
2050 if rs(i) > 0 then i=i+1: goto 2120
2060 rx(i) = rx(anum)
2070 ry(i) = ry(anum)
2080 rdx(i) = rdx(anum)
2090 rdy(i) = rdy(anum)
2100 rs(i) = rs(anum)
2110 anum = anum - 1
2120 if i<= anum then 2050
2130 return
2140 rem ************************* handle explosion dots
2150 for i=1 to emax
2160 if et(i) <= 0 then 2250
2170 ex(i) = ex(i) + edx(i)
2180 if ex(i)<0 then ex(i) = ex(i) + xsize
2190 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2200 ey(i) = ey(i) + edy(i)
2210 if ey(i)<0 then ey(i) = ey(i) + ysize
2220 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2230 et(i) = et(i) - 1
2240 disc ex(i), ey(i), 1.5
2250 next i
2260 return
2270 rem *********************** Add some explosion dots
2280 rem x, y is coord
2290 uc=15
2300 for u = 1 to emax
2310 if et(u) >0 then 2410
2320 et(u) = 50
2330 ex(u) = x
2340 ey(u) = y
2350 ur = rnd(0)*2 + 1
2360 ut = rnd(360)*3.1415928/180
2370 edx(u) = ur*cos(ut)
2380 edy(u) = ur*sin(ut)
2390 uc = uc - 1
2400 if uc = 0 then 2420
2410 next u
2420 return
2430 rem ********************* Check if ship destroyed
2440 if dead<>0 then 2560
2450 if anum = 0 then 2560
2460 for i = 1 to anum
2470 tx = shipx - rx(i)
2480 ty = shipy - ry(i)
2490 r = sqr(tx*tx+ty*ty)
2500 if r > rs(i) then 2550
2510 dead=1
2520 x=shipx
2530 y=shipy
2540 gosub 2270: gosub 2270: gosub 2270
2550 next i
2560 return
