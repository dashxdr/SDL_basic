10 home
20 clear
30 w=rnd(50)
40 x=rnd(1024)
50 y=rnd(768)
60 color 0,0,0
70 disc x,y,w*1.05
80 color rnd(128)+100,0,0
90 color 255,220,0
100 s=w*.03
110 disc x-s,y-s,w
120 for j=1 to 500
130 next
140 goto 30
