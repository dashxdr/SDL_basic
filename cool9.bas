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
440 gosub 2410: rem handle explosion dots
450 gosub 2700: rem check if ship destroyed
460 gosub 2850: rem housekeeping
470 gosub 3020: rem sounds
480 update
490 wt = sleep(.02)
500 if restart=1 then 50
510 goto 330
520 rem ************************   Handle ship movement
530 if dead<>0 then 790
540 fire=0
550 if key(402)=0 then 620
560 gosub 3180
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
1330 gosub 3120
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
2000 if missnum = 0 or anum = 0 then 2400
2010 for i=1 to missnum
2020 x = missx(i):y=missy(i)
2030 if misst(i) <= 0 then 2300
2040 for j=1 to anum
2050 if rs(j) = 0 then 2290
2060 tx = rx(j) - x
2070 ty = ry(j) - y
2080 r = sqr(tx*tx + ty*ty)
2090 if r>rs(j) then 2290
2100 dst1 = dst1 + ddst1
2110 gosub 3150
2120 gosub 2540
2130 misst(i) = 0
2140 rs(j) = rs(j)*.5
2150 if rs(j) < amin then rs(j) = 0:goto 2290
2160 s=sqr(rdx(j)*rdx(j) + rdy(j)*rdy(j))
2170 s = s * 2
2180 tx = s * tx/r
2190 ty = s * ty/r
2200 rdx(j) = -ty
2210 rdy(j) = tx
2220 anum = anum+1
2230 rx(anum) = rx(j)
2240 ry(anum) = ry(j)
2250 rdx(anum) = -rdx(j)
2260 rdy(anum) = -rdy(j)
2270 rs(anum) = rs(j)
2280 goto 2300
2290 next j
2300 next i
2310 i=1: goto 2390
2320 if rs(i) > 0 then i=i+1: goto 2390
2330 rx(i) = rx(anum)
2340 ry(i) = ry(anum)
2350 rdx(i) = rdx(anum)
2360 rdy(i) = rdy(anum)
2370 rs(i) = rs(anum)
2380 anum = anum - 1
2390 if i<= anum then 2320
2400 return
2410 rem ************************* handle explosion dots
2420 for i=1 to emax
2430 if et(i) <= 0 then 2520
2440 ex(i) = ex(i) + edx(i)
2450 if ex(i)<0 then ex(i) = ex(i) + xsize
2460 if ex(i)>=xsize then ex(i) = ex(i) - xsize
2470 ey(i) = ey(i) + edy(i)
2480 if ey(i)<0 then ey(i) = ey(i) + ysize
2490 if ey(i)>=ysize then ey(i) = ey(i) - ysize
2500 et(i) = et(i) - 1
2510 disc ex(i), ey(i), 1.5
2520 next i
2530 return
2540 rem *********************** Add some explosion dots
2550 rem x, y is coord
2560 uc=15
2570 for u = 1 to emax
2580 if et(u) >0 then 2680
2590 et(u) = 50 + rnd(10)
2600 ex(u) = x
2610 ey(u) = y
2620 ur = rnd(0)*2 + 1
2630 ut = rnd(360)*3.1415928/180
2640 edx(u) = ur*cos(ut)
2650 edy(u) = ur*sin(ut)
2660 uc = uc - 1
2670 if uc = 0 then 2690
2680 next u
2690 return
2700 rem ********************* Check if ship destroyed
2710 if dead<>0 then 2840
2720 if anum = 0 then 2840
2730 for i = 1 to anum
2740 tx = shipx - rx(i)
2750 ty = shipy - ry(i)
2760 r = sqr(tx*tx+ty*ty)
2770 if r > rs(i) then 2830
2780 gosub 3150
2790 dead=1
2800 x=shipx
2810 y=shipy
2820 gosub 2540: gosub 2540: gosub 2540
2830 next i
2840 return
2850 rem ********************** Housekeeping functions
2860 if dead=0 then 2980
2870 timer = timer + 1
2880 if timer <150 then 3010
2890 shipx = xsize/2: shipy = ysize/2: dx=0: dy=0: shipa=3.1415928/2
2900 if anum = 0 then 2970
2910 for i=1 to anum
2920 tx = rx(i) - shipx
2930 ty = ry(i) - shipy
2940 r=sqr(tx*tx + ty*ty)
2950 if r<xsize/4 or r<ysize/4 then 2980
2960 next i
2970 dead = 0:timer=0
2980 if anum<>0 then 3010
2990 timer = timer + 1
3000 if timer = 150 then timer = 0:gosub 1500
3010 return
3020 rem *************************** sounds
3030 st1 = st1 + dst1
3040 if st1 < 1 then 3080
3050 st1 = 0
3060 tone 1, freq(note(40 + tn1)), dur(.1), vol(20)
3070 tn1 = 1-tn1
3080 avol = avol - 4
3090 if avol<0 then avol=0: goto 3110
3100 tone 32, freq(5), dur(1), vol(avol)
3110 return
3120 rem *************** Shoot missile sound
3130 tone 2, freq(900), fmul(.989), dur(.15), vol(40)
3140 return
3150 rem *************** Asteroid explosion sound
3160 avol = 100
3170 return
3180 rem ************************   Ship thruster sound
3190 tone 31, freq(4), vol(50), dur(.2)
3200 return
