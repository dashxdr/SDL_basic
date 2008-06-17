10 home
20 cls
30 pen 5
40 k=sin(a)*127+127
50 a=a+.001
60 color rnd(256),0,0
70 circle rnd(xsize),rnd(ysize),rnd(50)
80 t=t+1
90 if t<5 then 40
100 t=0
110 sleep .001
120 goto 40
