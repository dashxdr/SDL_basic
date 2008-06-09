2 goto 50
3 clear
5 pen 5
10 k=sin(a)*127+127;
12 a=a+.001
14 color rnd(256),0,0
20 circle rnd(1024),rnd(768),rnd(50)
30 goto 10
50 clear
60 w=rnd(50)
80 x=rnd(1024)
90 y=rnd(768)
95 color 0,0,0
100 disc x,y,w*1.05
105 color rnd(128)+100,0,0
106 color 255,220,0
107 s=w*.03
110 disc x-s,y-s,w
115 for j=1 to 500
116 next
120 goto 60
