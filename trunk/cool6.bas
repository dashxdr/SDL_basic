10 home
20 dim r(200),g(200),b(200)
30 dim x(1000),y(1000)
40 gosub 160
50 gosub 330
60 i=1
70 clear
80 x(i)=mousex
90 y(i)=mousey
100 gosub 220
110 i=i+1
120 if i>1000 then i=i-1000
130 update
140 sleep .02
150 goto 70
160 for i=1 to 200
170 r(i) = rnd(256)
180 g(i) = rnd(256)
190 b(i) = rnd(256)
200 next i
210 return
220 k=i
230 for j=60 to 1 step -1
240 k=i-(j-1)*5
250 if k<1 then k=k+1000
260 color r(j),g(j),b(j)
270 disc x(k),y(k),20
280 color 0,0,0
290 pen 2
300 circle x(k),y(k),20
310 next j
320 return
330 for i=1 to 1000
340 x(i) = mousex
350 y(i) = mousey
360 next i
370 return
