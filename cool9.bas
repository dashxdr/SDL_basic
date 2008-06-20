10 dim missx(100),missy(100), missdx(100), missdy(100), misst(100): rem *** missiles
20 dim sx(100), sy(100):rem *** stars
30 dim rx(100), ry(100), rdx(100), rdy(100), rs(100)
40 dim ex(100), ey(100), edx(100), edy(100), et(100)
50 rem ****** restart here
60 tn1 = 0
70 avol = 0
80 ddst1 = .002
90 restart = 0
100 timer=0
110 dead = 0
120 wantm = 0
130 fc = 0
140 lastm = -20
150 dx = 0: dy = 0
160 emax = 100 : rem max explosion dots
170 for i=1 to emax:et(i) = 0: next i
180 missnum = 0
190 starnum = 60
200 for i=1 to starnum
210 sx(i) = rnd(xsize)
220 sy(i) = rnd(ysize)
230 next i
240 pi2=3.1415928*2.0
250 shipx = xsize/2 : rem x position of ship
260 shipy = ysize/2 : rem y position of ship
270 thrust=.2 : rem thrust
280 v=0: rem velocity
290 mv=7 : rem max velocity
300 da = .1: rem rotation speed
310 shipa=3.1415928/2: rem Angle ship is facing
320 gosub 1500
330 rem ************************   Main loop
340 cls
350 print wt
360 fc=fc+1
370 gosub 1430: rem stars
380 gosub 520: rem handle ship
390 gosub 990: rem hacks
400 gosub 1050: rem missiles
410 gosub 1300: rem handle firing missiles
420 gosub 1690: rem asteroids
430 gosub 1990: rem missiles hitting asteroids
440 gosub 2430: rem handle explosion dots
450 gosub 2720: rem check if ship destroyed
460 gosub 2890: rem housekeeping
470 gosub 3060: rem sounds
480 update
490 wt = sleep(.02)
500 if restart=1 then 50
510 goto 330
520 rem ************************   Handle ship movement
530 if dead<>0 then 790
540 fire=0
550 if key(402)=0 then 620
560 gosub 3220
570 dx = dx + thrust*cos(a)
580 dy = dy - thrust*sin(a)
590 v = sqr(dx*dx + dy*dy)
600 if v>mv then v=mv/v:dx = dx*v : dy = dy*v
610 fire = 1
620 if key(400) then shipa=shipa+da:if(shipa<0.0) then shipa=shipa+pi2
630 if key(401) then shipa=shipa-da:if(shipa>pi2) then shipa=shipa-pi2
640 shipx=shipx+dx
650 if shipx<0 then shipx=shipx+xsize
660 if shipx>= xsize then shipx=shipx-xsize
670 shipy=shipy+dy
680 if shipy<0 then shipy=shipy+ysize
690 if shipy>=ysize then shipy=shipy-ysize
700 a = shipa
710 x = shipx
720 y = shipy
730 gosub 800
740 if x>xsize/2 then fx=-xsize else fx=xsize
750 if y>ysize/2 then fy=-ysize else fy=ysize
760 x=x+fx:gosub 800
770 y=y+fy:gosub 800
780 x=x-fx:gosub 800
790 return
800 rem ************************   Draw ship
810 pen 2
820 as=2.5
830 r=20
840 r2=r*.8
850 color 255,255,255
860 move x+r*cos(a), y-r*sin(a)
870 line x+r2*cos(a+as), y-r2*sin(a+as)
880 line x+r2*cos(a-as), y-r2*sin(a-as)
890 line x+r*cos(a), y-r*sin(a)
900 if fire=0 then 980
910 if fc&1 = 0 then 980
920 r2=r*.7
930 move x-r2*cos(a), y+r2*sin(a)
940 r2=r2*2
950 color 255,190,0
960 pen 4
970 line x-r2*cos(a), y+r2*sin(a)
980 return
990 rem ************************  Hacks
1000 code = keycode
1010 if code=13 then x=xsize/2:y=ysize/2:dx=0:dy=0
1020 if code=410 and dead=0 then wantm=1: rem want to fire a missile
1030 if code=32 then restart=1
1040 return
1050 rem ************************ Handle missiles
1060 if missnum = 0 then 1290
1070 for i = 1 to missnum
1080 if misst(i) <= 0 then 1180
1090 missx(i) = missx(i) + missdx(i)
1100 if missx(i)<0 then missx(i) = missx(i) + xsize
1110 if missx(i)>=xsize then missx(i) = missx(i) - xsize
1120 missy(i) = missy(i) + missdy(i)
1130 if missy(i)<0 then missy(i) = missy(i) + ysize
1140 if missy(i)>=ysize then missy(i) = missy(i) - ysize
1150 color 255,255,255
1160 disc missx(i), missy(i), 2
1170 misst(i) = misst(i) - 1
1180 next i
1190 if misst(1) > 0 then 1290
1200 missnum = missnum - 1
1210 if missnum = 0 then 1290
1220 for i = 1 to missnum
1230 missx(i) = missx(i+1)
1240 missy(i) = missy(i+1)
1250 missdx(i) = missdx(i+1)
1260 missdy(i) = missdy(i+1)
1270 misst(i) = misst(i+1)
1280 next i
1290 return
1300 rem ************************   Fire missile
1310 if wantm = 0 then 1420
1320 if lastm+6 > fc then 1420
1330 gosub 3160
1340 lastm = fc
1350 missnum = missnum + 1
1360 misst(missnum) = 60
1370 missx(missnum) = x
1380 missy(missnum) = y
1390 mspeed = mv*1.2
1400 missdx(missnum) = dx + mspeed*cos(shipa)
1410 missdy(missnum) = dy - mspeed*sin(shipa)
1420 return
1430 rem ************************  Stars
1440 color 255,255,255
1450 wantm = 0
1460 for i=1 to starnum
1470 disc sx(i),sy(i),1
1480 next i
1490 return
1500 rem ************************ setup asteroids
1510 dst1 = .02
1520 anum = 4
1530 amin = 15
1540 for i = 1 to anum
1550 tx = rnd(xsize)
1560 ty = rnd(ysize)
1570 tx2 = tx - shipx:ty2 = ty - shipy
1580 r=sqr(tx2*tx2 + ty2*ty2)
1590 if r < xsize/2 or r < ysize/2 then 1550
1600 rx(i) = tx
1610 ry(i) = ty
1620 ta = rnd(360) * 3.1415928/180.0
1630 r = 1: rem asteroid initial speed
1640 rdx(i) = r*cos(ta)
1650 rdy(i) = r*sin(ta)
1660 rs(i) = amin*4
1670 next i
1680 return
1690 rem ************************ handle asteroids
1700 if anum = 0 then 1960
1710 color 255,255,255
1720 for i = 1 to anum
1730 rx(i) = rx(i) + rdx(i)
1740 if rx(i) < 0 then rx(i) = rx(i) + xsize
1750 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1760 ry(i) = ry(i) + rdy(i)
1770 if ry(i) < 0 then ry(i) = ry(i) + ysize
1780 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1790 x = rx(i): y = ry(i) : s = rs(i)
1800 pen s*.2
1810 b = s*1.3
1820 if x>b and x<xsize-b then fx=0 else if x<=b then fx=xsize else fx=-xsize
1830 if y>b and y<ysize-b then fy=0 else if y<=b then fy=ysize else fy=-ysize
1840 gosub 1970
1850 if fx=0 then 1890
1860 x=x+fx
1870 gosub 1970
1880 x=x-fx
1890 if fy=0 then 1950
1900 y=y+fy
1910 gosub 1970
1920 if fx=0 then 1950
1930 x=x+fx
1940 gosub 1970
1950 next i
1960 return
1970 circle x,y,s
1980 return
1990 rem *********************** handle missiles hitting asteroids
2000 if missnum = 0 or anum = 0 then 2420
2010 for i=1 to missnum
2020 x = missx(i):y=missy(i)
2030 if misst(i) <= 0 then 2320
2040 for j=1 to anum
2050 if rs(j) = 0 then 2310
2060 tx = abs(rx(j) - x)
2070 if tx>xsize/2 then tx=xsize-tx
2080 ty = abs(ry(j) - y)
2090 if ty>ysize/2 then ty=ysize-ty
2100 r = sqr(tx*tx + ty*ty)
2110 if r>rs(j) then 2310
2120 dst1 = dst1 + ddst1
2130 gosub 3190
2140 gosub 2560
2150 misst(i) = 0
2160 rs(j) = rs(j)*.5
2170 if rs(j) < amin then rs(j) = 0:goto 2310
2180 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
2190 s = s * 2
2200 tx = s * tx/r
2210 ty = s * ty/r
2220 rdx(j) = -ty
2230 rdy(j) = tx
2240 anum = anum+1
2250 rx(anum) = rx(j)
2260 ry(anum) = ry(j)
2270 rdx(anum) = -rdx(j)
2280 rdy(anum) = -rdy(j)
2290 rs(anum) = rs(j)
2300 goto 2320
2310 next j
2320 next i
2330 i=1: goto 2410
2340 if rs(i) > 0 then i=i+1: goto 2410
2350 rx(i) = rx(anum)
2360 ry(i) = ry(anum)
2370 rdx(i) = rdx(anum)
2380 rdy(i) = rdy(anum)
2390 rs(i) = rs(anum)
2400 anum = anum - 1
2410 if i<= anum then 2340
2420 return
2430 rem ************************* handle explosion dots
2440 for i=1 to emax
2450 if et(i) <= 0 then 2540
2460 ex(i) = ex(i) + edx(i)
2470 if ex(i)<0 then ex(i) = ex(i) + xsize
2480 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2490 ey(i) = ey(i) + edy(i)
2500 if ey(i)<0 then ey(i) = ey(i) + ysize
2510 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2520 et(i) = et(i) - 1
2530 disc ex(i), ey(i), 1.5
2540 next i
2550 return
2560 rem *********************** Add some explosion dots
2570 rem x, y is coord
2580 uc=15
2590 for u = 1 to emax
2600 if et(u) >0 then 2700
2610 et(u) = 50 + rnd(10)
2620 ex(u) = x
2630 ey(u) = y
2640 ur = rnd(0)*2 + 1
2650 ut = rnd(360)*3.1415928/180
2660 edx(u) = ur*cos(ut)
2670 edy(u) = ur*sin(ut)
2680 uc = uc - 1
2690 if uc = 0 then 2710
2700 next u
2710 return
2720 rem ********************* Check if ship destroyed
2730 if dead<>0 then 2880
2740 if anum = 0 then 2880
2750 for i = 1 to anum
2760 tx = abs(shipx - rx(i))
2770 if tx>xsize/2 then tx=xsize-tx
2780 ty = abs(shipy - ry(i))
2790 if ty>ysize/2 then ty=ysize-ty
2800 r = sqr(tx*tx+ty*ty)
2810 if r > rs(i)*1.2 then 2870
2820 gosub 3190
2830 dead=1
2840 x=shipx
2850 y=shipy
2860 gosub 2560: gosub 2560: gosub 2560
2870 next i
2880 return
2890 rem ********************** Housekeeping functions
2900 if dead=0 then 3020
2910 timer = timer + 1
2920 if timer <150 then 3050
2930 shipx = xsize/2: shipy = ysize/2: dx=0: dy=0: shipa=3.1415928/2
2940 if anum = 0 then 3010
2950 for i=1 to anum
2960 tx = rx(i) - shipx
2970 ty = ry(i) - shipy
2980 r=sqr(tx*tx + ty*ty)
2990 if r<xsize/4 or r<ysize/4 then 3020
3000 next i
3010 dead = 0:timer=0
3020 if anum<>0 then 3050
3030 timer = timer + 1
3040 if timer = 150 then timer = 0:gosub 1500
3050 return
3060 rem *************************** sounds
3070 st1 = st1 + dst1
3080 if st1 < 1 then 3120
3090 st1 = 0
3100 tone 1, freq(note(40 + tn1)), dur(.1), vol(20)
3110 tn1 = 1-tn1
3120 avol = avol - 4
3130 if avol<0 then avol=0: goto 3150
3140 tone 32, freq(5), dur(1), vol(avol)
3150 return
3160 rem *************** Shoot missile sound
3170 tone 2, freq(900), fmul(.989), dur(.15), vol(40), wtri
3180 return
3190 rem *************** Asteroid explosion sound
3200 avol = 100
3210 return
3220 rem ************************   Ship thruster sound
3230 tone 31, freq(4), vol(50), dur(.2)
3240 return
