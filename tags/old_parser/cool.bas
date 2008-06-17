10 t=0
20 home
30 cls
40 w=rnd(50)
50 s=3
60 x=rnd(1024)
70 y=rnd(768)
80 color 0,0,0
90 box x,y,w,w
100 color rnd(128)+100,0,0
110 box x-s,y-s,w,w
120 t=t+1
130 if t<5 then 40
140 t=0
150 sleep .001
160 goto 40
