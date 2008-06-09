10 dim r(200),g(200),b(200)
20 dim x(1000),y(1000)
25 gosub 150
26 gosub 300
30 i=1
40 clear
50 x(i)=mousex
60 y(i)=mousey
70 gosub 200
80 i=i+1
90 if i>1000 then i=i-1000
100 update
110 sleep .02
120 goto 40
150 for i=1 to 200
160 r(i) = rnd(256)
170 g(i) = rnd(256)
185 b(i) = rnd(256)
190 next i
195 return
200 k=i
210 for j=60 to 1 step -1
211 k=i-(j-1)*5
212 if k<1 then k=k+1000
215 color r(j),g(j),b(j)
220 disc x(k),y(k),20
250 next j
260 return
300 for i=1 to 1000
310 x(i) = mousex
320 y(i) = mousey
330 next i
340 return
