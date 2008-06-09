10 clear
20 pen 5
30 k=sin(a)*127+127;
40 a=a+.001
50 color rnd(256),0,0
60 circle rnd(1024),rnd(768),rnd(50)
70 goto 30
80 clear
90 w=rnd(50)
100 s=3
110 x=rnd(1024)
120 y=rnd(768)
130 color 0,0,0
140 disc x,y,w
150 color rnd(128)+100,0,0
160 disc x-s,y-s,w
170 for j=1 to 500
180 next
190 goto 90
