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
430 gosub 1940: rem missiles hitting asteroids
440 gosub 2360: rem handle explosion dots
450 gosub 2650: rem check if ship destroyed
460 gosub 2800: rem housekeeping
470 gosub 2970: rem sounds
480 update
490 wt = sleep(.02)
500 if restart=1 then 50
510 goto 330
520 rem ************************   Handle ship movement
530 if dead<>0 then 790
540 fire=0
550 if key(402)=0 then 620
560 gosub 3130
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
1020 if code=410 then wantm=1: rem want to fire a missile
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
1330 gosub 3070
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
1700 if anum = 0 then 1910
1710 color 255,255,255
1720 for i = 1 to anum
1730 rx(i) = rx(i) + rdx(i)
1740 if rx(i) < 0 then rx(i) = rx(i) + xsize
1750 if rx(i) >= xsize then rx(i) = rx(i) - xsize
1760 ry(i) = ry(i) + rdy(i)
1770 if ry(i) < 0 then ry(i) = ry(i) + ysize
1780 if ry(i) >= ysize then ry(i) = ry(i) - ysize
1790 x = rx(i): y = ry(i) : s = rs(i)
1800 if x>xsize/2 then fx=-xsize else fx=xsize
1810 if y>ysize/2 then fy=-ysize else fy=ysize
1820 pen rs(i)*.2
1830 gosub 1920
1840 x=x+fx
1850 gosub 1920
1860 y=y+fy
1870 gosub 1920
1880 x=x-fx
1890 gosub 1920
1900 next i
1910 return
1920 circle x,y,s
1930 return
1940 rem *********************** handle missiles hitting asteroids
1950 if missnum = 0 or anum = 0 then 2350
1960 for i=1 to missnum
1970 x = missx(i):y=missy(i)
1980 if misst(i) <= 0 then 2250
1990 for j=1 to anum
2000 if rs(j) = 0 then 2240
2010 tx = rx(j) - x
2020 ty = ry(j) - y
2030 r = sqr(tx*tx + ty*ty)
2040 if r>rs(j) then 2240
2050 dst1 = dst1 + ddst1
2060 gosub 3100
2070 gosub 2490
2080 misst(i) = 0
2090 rs(j) = rs(j)*.5
2100 if rs(j) < amin then rs(j) = 0:goto 2240
2110 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
2120 s = s * 2
2130 tx = s * tx/r
2140 ty = s * ty/r
2150 rdx(j) = -ty
2160 rdy(j) = tx
2170 anum = anum+1
2180 rx(anum) = rx(j)
2190 ry(anum) = ry(j)
2200 rdx(anum) = -rdx(j)
2210 rdy(anum) = -rdy(j)
2220 rs(anum) = rs(j)
2230 goto 2250
2240 next j
2250 next i
2260 i=1: goto 2340
2270 if rs(i) > 0 then i=i+1: goto 2340
2280 rx(i) = rx(anum)
2290 ry(i) = ry(anum)
2300 rdx(i) = rdx(anum)
2310 rdy(i) = rdy(anum)
2320 rs(i) = rs(anum)
2330 anum = anum - 1
2340 if i<= anum then 2270
2350 return
2360 rem ************************* handle explosion dots
2370 for i=1 to emax
2380 if et(i) <= 0 then 2470
2390 ex(i) = ex(i) + edx(i)
2400 if ex(i)<0 then ex(i) = ex(i) + xsize
2410 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2420 ey(i) = ey(i) + edy(i)
2430 if ey(i)<0 then ey(i) = ey(i) + ysize
2440 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2450 et(i) = et(i) - 1
2460 disc ex(i), ey(i), 1.5
2470 next i
2480 return
2490 rem *********************** Add some explosion dots
2500 rem x, y is coord
2510 uc=15
2520 for u = 1 to emax
2530 if et(u) >0 then 2630
2540 et(u) = 50 + rnd(10)
2550 ex(u) = x
2560 ey(u) = y
2570 ur = rnd(0)*2 + 1
2580 ut = rnd(360)*3.1415928/180
2590 edx(u) = ur*cos(ut)
2600 edy(u) = ur*sin(ut)
2610 uc = uc - 1
2620 if uc = 0 then 2640
2630 next u
2640 return
2650 rem ********************* Check if ship destroyed
2660 if dead<>0 then 2790
2670 if anum = 0 then 2790
2680 for i = 1 to anum
2690 tx = shipx - rx(i)
2700 ty = shipy - ry(i)
2710 r = sqr(tx*tx+ty*ty)
2720 if r > rs(i) then 2780
2730 gosub 3100
2740 dead=1
2750 x=shipx
2760 y=shipy
2770 gosub 2490: gosub 2490: gosub 2490
2780 next i
2790 return
2800 rem ********************** Housekeeping functions
2810 if dead=0 then 2930
2820 timer = timer + 1
2830 if timer <150 then 2960
2840 shipx = xsize/2: shipy = ysize/2: dx=0: dy=0: a=3.1415928/2
2850 if anum = 0 then 2920
2860 for i=1 to anum
2870 tx = rx(i) - shipx
2880 ty = ry(i) - shipy
2890 r=sqr(tx*tx + ty*ty)
2900 if r<xsize/4 or r<ysize/4 then 2930
2910 next i
2920 dead = 0:timer=0
2930 if anum<>0 then 2960
2940 timer = timer + 1
2950 if timer = 150 then timer = 0:gosub 1500
2960 return
2970 rem *************************** sounds
2980 st1 = st1 + dst1
2990 if st1 < 1 then 3030
3000 st1 = 0
3010 tone 1, freq(note(40 + tn1)), dur(.1), vol(20)
3020 tn1 = 1-tn1
3030 avol = avol - 4
3040 if avol<0 then avol=0: goto 3060
3050 tone 32, freq(5), dur(1), vol(avol)
3060 return
3070 rem *************** Shoot missile sound
3080 tone 2, freq(900), fmul(.989), dur(.15), vol(40)
3090 return
3100 rem *************** Asteroid explosion sound
3110 avol = 100
3120 return
3130 rem ************************   Ship thruster sound
3140 tone 31, freq(4), vol(50), dur(.1)
3150 return
