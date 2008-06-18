10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 dim ex(100), ey(100), edx(100), edy(100), et(100)
50 rem ****** restart here
60 tn1 = 0
70 ddst1 = .002
80 restart = 0
90 timer=0
100 dead = 0
110 wantm = 0
120 fc = 0
130 lastm = -20
140 dx = 0: dy = 0
150 emax = 100 : rem max explosion dots
160 for i=1 to emax:et(i) = 0: next i
170 missnum = 0
180 starnum = 60
190 for i=1 to starnum
200 sx(i) = rnd(xsize)
210 sy(i) = rnd(ysize)
220 next i
230 pi2=3.1415928*2.0
240 shipx = xsize/2 : rem x position of ship
250 shipy = ysize/2 : rem y position of ship
260 thrust=.2 : rem thrust
270 v=0: rem velocity
280 mv=7 : rem max velocity
290 da = .1: rem rotation speed
300 shipa=3.1415928/2: rem Angle ship is facing
310 gosub 1480
320 rem ************************   Main loop
330 cls
340 print wt
350 fc=fc+1
360 gosub 1410: rem stars
370 gosub 510: rem handle ship
380 gosub 970: rem hacks
390 gosub 1030: rem missiles
400 gosub 1280: rem handle firing missiles
410 gosub 1670: rem asteroids
420 gosub 1920: rem missiles hitting asteroids
430 gosub 2330: rem handle explosion dots
440 gosub 2620: rem check if ship destroyed
450 gosub 2760: rem housekeeping
460 gosub 2930: rem sounds
470 update
480 wt = sleep(.02)
490 if restart=1 then 50
500 goto 320
510 rem ************************   Handle ship movement
520 if dead<>0 then 770
530 fire=0
540 if key(402)=0 then 600
550 dx = dx + thrust*cos(a)
560 dy = dy - thrust*sin(a)
570 v = sqr(dx*dx + dy*dy)
580 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
590 fire = 1
600 if key(400) then shipa=shipa+da:if(shipa<0.0) then shipa=shipa+pi2
610 if key(401) then shipa=shipa-da:if(shipa>pi2) then shipa=shipa-pi2
620 shipx=shipx+dx
630 if shipx<0 then shipx=shipx+xsize
640 if shipx>= xsize then shipx=shipx-xsize
650 shipy=shipy+dy
660 if shipy<0 then shipy=shipy+ysize
670 if shipy>=ysize then shipy=shipy-ysize
680 a = shipa
690 x = shipx
700 y = shipy
710 gosub 780
720 if x>xsize/2 then fx=-xsize else fx=xsize
730 if y>ysize/2 then fy=-ysize else fy=ysize
740 x=x+fx:gosub 780
750 y=y+fy:gosub 780
760 x=x-fx:gosub 780
770 return
780 rem ************************   Draw ship
790 pen 2
800 as=2.5
810 r=20
820 r2=r*.8
830 color 255,255,255
840 move x+r*cos(a), y-r*sin(a)
850 line x+r2*cos(a+as), y-r2*sin(a+as)
860 line x+r2*cos(a-as), y-r2*sin(a-as)
870 line x+r*cos(a), y-r*sin(a)
880 if fire=0 then 960
890 if fc&1 = 0 then 960
900 r2=r*.7
910 move x-r2*cos(a), y+r2*sin(a)
920 r2=r2*2
930 color 255,190,0
940 pen 4
950 line x-r2*cos(a), y+r2*sin(a)
960 return
970 rem ************************  Hacks
980 code = keycode
990 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
1000 if code=410 then wantm=1: rem want to fire a missile
1010 if code=32 then restart=1
1020 return
1030 rem ************************ Handle missiles
1040 if missnum = 0 then 1270
1050 for i = 1 to missnum
1060 if misst(i) <= 0 then 1160
1070 missx(i) = missx(i) + missdx(i)
1080 if missx(i)<0 then missx(i) = missx(i) + xsize
1090 if missx(i)>=xsize then missx(i) = missx(i) - xsize
1100 missy(i) = missy(i) + missdy(i)
1110 if missy(i)<0 then missy(i) = missy(i) + ysize
1120 if missy(i)>=ysize then missy(i) = missy(i) - ysize
1130 color 255,255,255
1140 disc missx(i), missy(i), 2
1150 misst(i) = misst(i) - 1
1160 next i
1170 if misst(1) > 0 then 1270
1180 missnum = missnum - 1
1190 if missnum = 0 then 1270
1200 for i = 1 to missnum
1210 missx(i) = missx(i+1)
1220 missy(i) = missy(i+1)
1230 missdx(i) = missdx(i+1)
1240 missdy(i) = missdy(i+1)
1250 misst(i) = misst(i+1)
1260 next i
1270 return
1280 rem ************************   Fire missile
1290 if wantm = 0 then 1400
1300 if lastm+6 > fc then 1400
1310 gosub 3000
1320 lastm = fc
1330 missnum = missnum + 1
1340 misst(missnum) = 60
1350 missx(missnum) = x
1360 missy(missnum) = y
1370 mspeed = mv*1.2
1380 missdx(missnum) = dx + mspeed*cos(shipa)
1390 missdy(missnum) = dy - mspeed*sin(shipa)
1400 return
1410 rem ************************  Stars
1420 color 255,255,255
1430 wantm = 0
1440 for i=1 to starnum
1450 disc sx(i),sy(i),1
1460 next i
1470 return
1480 rem ************************ setup asteroids
1490 dst1 = .02
1500 anum = 4
1510 amin = 15
1520 for i = 1 to anum
1530 tx = rnd(xsize)
1540 ty = rnd(ysize)
1550 tx2 = tx - shipx:ty2 = ty - shipy
1560 r=sqr(tx2*tx2 + ty2*ty2)
1570 if r < xsize/2 or r < ysize/2 then 1530
1580 rx(i) = tx
1590 ry(i) = ty
1600 ta = rnd(360) * 3.1415928/180.0
1610 r = 1: rem asteroid initial speed
1620 rdx(i) = r*cos(ta)
1630 rdy(i) = r*sin(ta)
1640 rs(i) = amin*4
1650 next i
1660 return
1670 rem ************************ handle asteroids
1680 if anum = 0 then 1890
1690 color 255,255,255
1700 for i = 1 to anum
1710 rx(i) = rx(i) + rdx(i)
1720 if rx(i) < 0 then rx(i) = rx(i) + xsize
1730 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1740 ry(i) = ry(i) + rdy(i)
1750 if ry(i) < 0 then ry(i) = ry(i) + ysize
1760 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1770 x = rx(i): y = ry(i) : s = rs(i)
1780 if x>xsize/2 then fx=-xsize else fx=xsize
1790 if y>ysize/2 then fy=-ysize else fy=ysize
1800 pen rs(i)*.2
1810 gosub 1900
1820 x=x+fx
1830 gosub 1900
1840 y=y+fy
1850 gosub 1900
1860 x=x-fx
1870 gosub 1900
1880 next i
1890 return
1900 circle x,y,s
1910 return
1920 rem *********************** handle missiles hitting asteroids
1930 if missnum = 0 or anum = 0 then 2320
1940 for i=1 to missnum
1950 x = missx(i):y=missy(i)
1960 if misst(i) <= 0 then 2220
1970 for j=1 to anum
1980 if rs(j) = 0 then 2210
1990 tx = rx(j) - x
2000 ty = ry(j) - y
2010 r = sqr(tx*tx + ty*ty)
2020 if r>rs(j) then 2210
2030 dst1 = dst1 + ddst1
2040 gosub 2460
2050 misst(i) = 0
2060 rs(j) = rs(j)*.5
2070 if rs(j) < amin then rs(j) = 0:goto 2210
2080 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
2090 s = s * 2
2100 tx = s * tx/r
2110 ty = s * ty/r
2120 rdx(j) = -ty
2130 rdy(j) = tx
2140 anum = anum+1
2150 rx(anum) = rx(j)
2160 ry(anum) = ry(j)
2170 rdx(anum) = -rdx(j)
2180 rdy(anum) = -rdy(j)
2190 rs(anum) = rs(j)
2200 goto 2220
2210 next j
2220 next i
2230 i=1: goto 2310
2240 if rs(i) > 0 then i=i+1: goto 2310
2250 rx(i) = rx(anum)
2260 ry(i) = ry(anum)
2270 rdx(i) = rdx(anum)
2280 rdy(i) = rdy(anum)
2290 rs(i) = rs(anum)
2300 anum = anum - 1
2310 if i<= anum then 2240
2320 return
2330 rem ************************* handle explosion dots
2340 for i=1 to emax
2350 if et(i) <= 0 then 2440
2360 ex(i) = ex(i) + edx(i)
2370 if ex(i)<0 then ex(i) = ex(i) + xsize
2380 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2390 ey(i) = ey(i) + edy(i)
2400 if ey(i)<0 then ey(i) = ey(i) + ysize
2410 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2420 et(i) = et(i) - 1
2430 disc ex(i), ey(i), 1.5
2440 next i
2450 return
2460 rem *********************** Add some explosion dots
2470 rem x, y is coord
2480 uc=15
2490 for u = 1 to emax
2500 if et(u) >0 then 2600
2510 et(u) = 50 + rnd(10)
2520 ex(u) = x
2530 ey(u) = y
2540 ur = rnd(0)*2 + 1
2550 ut = rnd(360)*3.1415928/180
2560 edx(u) = ur*cos(ut)
2570 edy(u) = ur*sin(ut)
2580 uc = uc - 1
2590 if uc = 0 then 2610
2600 next u
2610 return
2620 rem ********************* Check if ship destroyed
2630 if dead<>0 then 2750
2640 if anum = 0 then 2750
2650 for i = 1 to anum
2660 tx = shipx - rx(i)
2670 ty = shipy - ry(i)
2680 r = sqr(tx*tx+ty*ty)
2690 if r > rs(i) then 2740
2700 dead=1
2710 x=shipx
2720 y=shipy
2730 gosub 2460: gosub 2460: gosub 2460
2740 next i
2750 return
2760 rem ********************** Housekeeping functions
2770 if dead=0 then 2890
2780 timer = timer + 1
2790 if timer <150 then 2920
2800 shipx = xsize/2: shipy = ysize/2: dx=0: dy=0: a=3.1415928/2
2810 if anum = 0 then 2880
2820 for i=1 to anum
2830 tx = rx(i) - shipx
2840 ty = ry(i) - shipy
2850 r=sqr(tx*tx + ty*ty)
2860 if r<xsize/4 or r<ysize/4 then 2890
2870 next i
2880 dead = 0:timer=0
2890 if anum<>0 then 2920
2900 timer = timer + 1
2910 if timer = 150 then timer = 0:gosub 1480
2920 return
2930 rem *************************** sounds
2940 st1 = st1 + dst1
2950 if st1 < 1 then 2990
2960 st1 = 0
2970 tone 1, freq(note(40 + tn1)), dur(.1), vol(20)
2980 tn1 = 1-tn1
2990 return
3000 rem *************** Shoot missile sound
3010 tone 2, freq(900), fmul(.989), dur(.15), vol(40)
3020 return
