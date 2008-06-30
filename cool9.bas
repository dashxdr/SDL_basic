10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 dim ex(100), ey(100), edx(100), edy(100), et(100)
50 rem ****** restart here
60 gx=xsize*40/100:gy=ysize/2:gs=0: rem *******  Black hole
70 tn1 = 0
80 avol = 0
90 ddst1 = .002
100 restart = 0
110 timer=0
120 dead = 0
130 wantm = 0
140 fc = 0
150 lastm = -20
160 dx = 0: dy = 0
170 emax = 100 : rem max explosion dots
180 for i=1 to emax:et(i) = 0: next i
190 missnum = 0
200 starnum = 60
210 for i=1 to starnum
220 sx(i) = rnd(xsize)
230 sy(i) = rnd(ysize)
240 next i
250 pi2=3.1415928*2.0
260 shipx = xsize/2 : rem x position of ship
270 shipy = ysize/2 : rem y position of ship
280 thrust=.2 : rem thrust
290 v=0: rem velocity
300 mv=7 : rem max velocity
310 da = .1: rem rotation speed
320 shipa=3.1415928/2: rem Angle ship is facing
330 gosub 1520
340 rem ************************   Main loop
350 cls
360 print wt
370 fc=fc+1
380 gosub 1450: rem stars
390 gosub 540: rem handle ship
400 gosub 1010: rem hacks
410 gosub 1070: rem missiles
420 gosub 1320: rem handle firing missiles
430 gosub 1710: rem asteroids
440 gosub 2010: rem missiles hitting asteroids
450 gosub 2450: rem handle explosion dots
460 gosub 2740: rem check if ship destroyed
470 gosub 2910: rem housekeeping
480 gosub 3080: rem sounds
490 gosub 3270: rem Deal with black hole
500 update
510 wt = sleep(.02)
520 if restart=1 then 50
530 goto 340
540 rem ************************   Handle ship movement
550 if dead<>0 then 810
560 fire=0
570 if key(402)=0 then 640
580 gosub 3240
590 dx = dx + thrust*cos(a)
600 dy = dy - thrust*sin(a)
610 v = sqr(dx*dx + dy*dy)
620 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
630 fire = 1
640 if key(400) then shipa=shipa+da:if(shipa<0.0) then shipa=shipa+pi2
650 if key(401) then shipa=shipa-da:if(shipa>pi2) then shipa=shipa-pi2
660 shipx=shipx+dx
670 if shipx<0 then shipx=shipx+xsize
680 if shipx>= xsize then shipx=shipx-xsize
690 shipy=shipy+dy
700 if shipy<0 then shipy=shipy+ysize
710 if shipy>=ysize then shipy=shipy-ysize
720 a = shipa
730 x = shipx
740 y = shipy
750 gosub 820
760 if x>xsize/2 then fx=-xsize else fx=xsize
770 if y>ysize/2 then fy=-ysize else fy=ysize
780 x=x+fx:gosub 820
790 y=y+fy:gosub 820
800 x=x-fx:gosub 820
810 return
820 rem ************************   Draw ship
830 pen 2
840 as=2.5
850 r=20
860 r2=r*.8
870 color 255,255,255
880 move x+r*cos(a), y-r*sin(a)
890 line x+r2*cos(a+as), y-r2*sin(a+as)
900 line x+r2*cos(a-as), y-r2*sin(a-as)
910 line x+r*cos(a), y-r*sin(a)
920 if fire=0 then 1000
930 if fc&1 = 0 then 1000
940 r2=r*.7
950 move x-r2*cos(a), y+r2*sin(a)
960 r2=r2*2
970 color 255,190,0
980 pen 4
990 line x-r2*cos(a), y+r2*sin(a)
1000 return
1010 rem ************************  Hacks
1020 code = keycode
1030 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
1040 if code=410 and dead=0 then wantm=1: rem want to fire a missile
1050 if code=32 then restart=1
1060 return
1070 rem ************************ Handle missiles
1080 if missnum = 0 then 1310
1090 for i = 1 to missnum
1100 if misst(i) <= 0 then 1200
1110 missx(i) = missx(i) + missdx(i)
1120 if missx(i)<0 then missx(i) = missx(i) + xsize
1130 if missx(i)>=xsize then missx(i) = missx(i) - xsize
1140 missy(i) = missy(i) + missdy(i)
1150 if missy(i)<0 then missy(i) = missy(i) + ysize
1160 if missy(i)>=ysize then missy(i) = missy(i) - ysize
1170 color 255,255,255
1180 disc missx(i), missy(i), 2
1190 misst(i) = misst(i) - 1
1200 next i
1210 if misst(1) > 0 then 1310
1220 missnum = missnum - 1
1230 if missnum = 0 then 1310
1240 for i = 1 to missnum
1250 missx(i) = missx(i+1)
1260 missy(i) = missy(i+1)
1270 missdx(i) = missdx(i+1)
1280 missdy(i) = missdy(i+1)
1290 misst(i) = misst(i+1)
1300 next i
1310 return
1320 rem ************************   Fire missile
1330 if wantm = 0 then 1440
1340 if lastm+6 > fc then 1440
1350 gosub 3180
1360 lastm = fc
1370 missnum = missnum + 1
1380 misst(missnum) = 60
1390 missx(missnum) = x
1400 missy(missnum) = y
1410 mspeed = mv*1.2
1420 missdx(missnum) = dx + mspeed*cos(shipa)
1430 missdy(missnum) = dy - mspeed*sin(shipa)
1440 return
1450 rem ************************  Stars
1460 color 255,255,255
1470 wantm = 0
1480 for i=1 to starnum
1490 disc sx(i),sy(i),1
1500 next i
1510 return
1520 rem ************************ setup asteroids
1530 dst1 = .02
1540 anum = 4
1550 amin = 15
1560 for i = 1 to anum
1570 tx = rnd(xsize)
1580 ty = rnd(ysize)
1590 tx2 = tx - shipx:ty2 = ty - shipy
1600 r=sqr(tx2*tx2 + ty2*ty2)
1610 if r < xsize/2 or r < ysize/2 then 1570
1620 rx(i) = tx
1630 ry(i) = ty
1640 ta = rnd(360) * 3.1415928/180.0
1650 r = 1: rem asteroid initial speed
1660 rdx(i) = r*cos(ta)
1670 rdy(i) = r*sin(ta)
1680 rs(i) = amin*4
1690 next i
1700 return
1710 rem ************************ handle asteroids
1720 if anum = 0 then 1980
1730 color 255,255,255
1740 for i = 1 to anum
1750 rx(i) = rx(i) + rdx(i)
1760 if rx(i) < 0 then rx(i) = rx(i) + xsize
1770 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1780 ry(i) = ry(i) + rdy(i)
1790 if ry(i) < 0 then ry(i) = ry(i) + ysize
1800 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1810 x = rx(i): y = ry(i) : s = rs(i)
1820 pen s*.2
1830 b = s*1.3
1840 if x>b and x<xsize-b then fx=0 else if x<=b then fx=xsize else fx=-xsize
1850 if y>b and y<ysize-b then fy=0 else if y<=b then fy=ysize else fy=-ysize
1860 gosub 1990
1870 if fx=0 then 1910
1880 x=x+fx
1890 gosub 1990
1900 x=x-fx
1910 if fy=0 then 1970
1920 y=y+fy
1930 gosub 1990
1940 if fx=0 then 1970
1950 x=x+fx
1960 gosub 1990
1970 next i
1980 return
1990 circle x,y,s
2000 return
2010 rem *********************** handle missiles hitting asteroids
2020 if missnum = 0 or anum = 0 then 2440
2030 for i=1 to missnum
2040 x = missx(i):y=missy(i)
2050 if misst(i) <= 0 then 2340
2060 for j=1 to anum
2070 if rs(j) = 0 then 2330
2080 tx = abs(rx(j) - x)
2090 if tx>xsize/2 then tx=xsize-tx
2100 ty = abs(ry(j) - y)
2110 if ty>ysize/2 then ty=ysize-ty
2120 r = sqr(tx*tx + ty*ty)
2130 if r>rs(j) then 2330
2140 dst1 = dst1 + ddst1
2150 gosub 3210
2160 gosub 2580
2170 misst(i) = 0
2180 rs(j) = rs(j)*.5
2190 if rs(j) < amin then rs(j) = 0:goto 2330
2200 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
2210 s = s * 2
2220 tx = s * tx/r
2230 ty = s * ty/r
2240 rdx(j) = -ty
2250 rdy(j) = tx
2260 anum = anum+1
2270 rx(anum) = rx(j)
2280 ry(anum) = ry(j)
2290 rdx(anum) = -rdx(j)
2300 rdy(anum) = -rdy(j)
2310 rs(anum) = rs(j)
2320 goto 2340
2330 next j
2340 next i
2350 i=1: goto 2430
2360 if rs(i) > 0 then i=i+1: goto 2430
2370 rx(i) = rx(anum)
2380 ry(i) = ry(anum)
2390 rdx(i) = rdx(anum)
2400 rdy(i) = rdy(anum)
2410 rs(i) = rs(anum)
2420 anum = anum - 1
2430 if i<= anum then 2360
2440 return
2450 rem ************************* handle explosion dots
2460 for i=1 to emax
2470 if et(i) <= 0 then 2560
2480 ex(i) = ex(i) + edx(i)
2490 if ex(i)<0 then ex(i) = ex(i) + xsize
2500 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2510 ey(i) = ey(i) + edy(i)
2520 if ey(i)<0 then ey(i) = ey(i) + ysize
2530 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2540 et(i) = et(i) - 1
2550 disc ex(i), ey(i), 1.5
2560 next i
2570 return
2580 rem *********************** Add some explosion dots
2590 rem x, y is coord
2600 uc=15
2610 for u = 1 to emax
2620 if et(u) >0 then 2720
2630 et(u) = 50 + rnd(10)
2640 ex(u) = x
2650 ey(u) = y
2660 ur = rnd(0)*2 + 1
2670 ut = rnd(360)*3.1415928/180
2680 edx(u) = ur*cos(ut)
2690 edy(u) = ur*sin(ut)
2700 uc = uc - 1
2710 if uc = 0 then 2730
2720 next u
2730 return
2740 rem ********************* Check if ship destroyed
2750 if dead<>0 then 2900
2760 if anum = 0 then 2900
2770 for i = 1 to anum
2780 tx = abs(shipx - rx(i))
2790 if tx>xsize/2 then tx=xsize-tx
2800 ty = abs(shipy - ry(i))
2810 if ty>ysize/2 then ty=ysize-ty
2820 r = sqr(tx*tx+ty*ty)
2830 if r > rs(i)*1.2 then 2890
2840 gosub 3210
2850 dead=1
2860 x=shipx
2870 y=shipy
2880 gosub 2580: gosub 2580: gosub 2580
2890 next i
2900 return
2910 rem ********************** Housekeeping functions
2920 if dead=0 then 3040
2930 timer = timer + 1
2940 if timer <150 then 3070
2950 shipx = xsize/2: shipy = ysize/2: dx=0: dy=0: shipa=3.1415928/2
2960 if anum = 0 then 3030
2970 for i=1 to anum
2980 tx = rx(i) - shipx
2990 ty = ry(i) - shipy
3000 r=sqr(tx*tx + ty*ty)
3010 if r<xsize/4 or r<ysize/4 then 3040
3020 next i
3030 dead = 0:timer=0
3040 if anum<>0 then 3070
3050 timer = timer + 1
3060 if timer = 150 then timer = 0:gosub 1520
3070 return
3080 rem *************************** sounds
3090 st1 = st1 + dst1
3100 if st1 < 1 then 3140
3110 st1 = 0
3120 tone 1, freq(note(40 + tn1)), dur(.1), vol(20)
3130 tn1 = 1-tn1
3140 avol = avol - 4
3150 if avol<0 then avol=0: goto 3170
3160 tone 32, freq(5), dur(1), vol(avol)
3170 return
3180 rem *************** Shoot missile sound
3190 tone 2, freq(900), fmul(.989), dur(.15), vol(40), wtri
3200 return
3210 rem *************** Asteroid explosion sound
3220 avol = 100
3230 return
3240 rem ************************   Ship thruster sound
3250 tone 31, freq(4), vol(50), dur(.2)
3260 return
3270 rem ***********************   Deal with gravity
3280 if gs = 0 then 3450
3290 color 255,205,0:disc gx, gy, 10
3300 if dead<>0 then 3330
3310 x=shipx:y=shipy:gosub 3460
3320 dx = dx + gdx:dy = dy + gdy
3330 if missnum=0 then 3390
3340 for i=1 to missnum
3350 x=missx(i):y=missy(i):gosub 3460
3360 missdx(i) = missdx(i) + gdx
3370 missdy(i) = missdy(i) + gdy
3380 next i
3390 if anum=0 then 3450
3400 for i=1 to anum
3410 x=rx(i):y=ry(i):gosub 3460
3420 rdx(i) = rdx(i) + gdx
3430 rdy(i) = rdy(i) + gdy
3440 next i
3450 return
3460 rem **********************   Gravity computations.
3470 rem **********************   Uses x, y, gx, gy, gs
3480 rem **********************   computes gdx, gdy
3490 tx = abs(x-gx): if tx>xsize/2 then tx=xsize - tx
3500 ty = abs(y-gy): if ty>ysize/2 then ty=ysize - ty
3510 t = tx*tx + ty*ty
3520 ts = sqr(t)
3530 if ts<5 then t = -t
3540 if t<>0 then t = gs / (t*ts)
3550 gdx = t*(gx-x)
3560 gdy = t*(gy-y)
3570 return
