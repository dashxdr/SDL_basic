10 home
20 cls
30 w=rnd(50)
40 x=rnd(xsize)
50 y=rnd(ysize)
60 color 0,0,0
70 disc x,y,w*1.05
80 color rnd(128)+100,0,0
90 color 255,220,0
100 s=w*.03
110 disc x-s,y-s,w
120 t=t+1
130 if t<5 then 30
140 t=0
150 sleep .001
160 goto 30
