10 home
20 cls
30 w=rnd(50)
40 s=3
50 x=rnd(xsize)
60 y=rnd(ysize)
70 color 0,0,0
80 disc x,y,w
90 color rnd(128)+100,0,0
100 disc x-s,y-s,w
110 t=t+1
120 if t<5 then 30
130 t=0
140 sleep .001
150 goto 30
