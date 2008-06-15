10 home
20 rr=40 : rem radius of disc
30 dim r(200),g(200),b(200)
40 dim x(1000),y(1000)
50 gosub 170
60 gosub 340
70 i=1
80 cls
90 x(i)=mousex
100 y(i)=mousey
110 gosub 230
120 i=i+1
130 if i>1000 then i=i-1000
140 update
150 sleep .02
160 goto 80
170 for i=1 to 200
180 r(i) = rnd(256)
190 g(i) = rnd(256)
200 b(i) = rnd(256)
210 next i
220 return
230 k=i
240 for j=60 to 1 step -1
250 k=i-(j-1)*5
260 if k<1 then k=k+1000
270 color r(j),g(j),b(j)
280 disc x(k),y(k),rr
290 color 0,0,0
300 pen 2
310 circle x(k),y(k),rr
320 next j
330 return
340 for i=1 to 1000
350 x(i) = mousex
360 y(i) = mousey
370 next i
380 return
