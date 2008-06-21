10 cls
20 x=rnd(xsize)
30 y=rnd(ysize)
40 r=rnd(100)
50 f=255/r
60 for i=r to 0 step -1
70 color 255-f*i,0,0
80 disc x,y,i
90 next i
100 goto 20
