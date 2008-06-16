10 pi2=3.1415928*2.0
20 x = xsize/2 : rem x position of ship
30 y = ysize/2 : rem y position of ship
40 t=.1 : rem thrust
50 v=0: rem velocity
60 mv=1 : rem max velocity
70 da = .1: rem rotation speed
80 a=3.1415928/2
90 cls
100 fc=fc+1
110 gosub 160
120 gosub 400
130 print sleep(.02);
140 update
150 goto 90
160 rem ************************   Handle ship
170 if key(402) then dx=dx + t*cos(a): dy = dy - t*sin(a):fire=1 else fire=0
180 if key(400) then a=a+da:if(a<0.0) then a=a+pi2
190 if key(401) then a=a-da:if(a>pi2) then a=a-pi2
200 pen 2
210 as=2.5
220 r=20
230 r2=r*.8
240 color 255,255,255
250 x=x+dx
260 y=y+dy
270 move x+r*cos(a), y-r*sin(a)
280 line x+r2*cos(a+as), y-r2*sin(a+as)
290 line x+r2*cos(a-as), y-r2*sin(a-as)
300 line x+r*cos(a), y-r*sin(a)
310 if fire=0 then 390
320 if fc&1 = 0 then 390
330 r2=r*.7
340 move x-r2*cos(a), y+r2*sin(a)
350 r2=r2*2
360 color 255,190,0
370 pen 4
380 line x-r2*cos(a), y+r2*sin(a)
390 return
400 if key(13) then x=xsize/2:y=ysize/2:dx=0:dy=0
410 return
