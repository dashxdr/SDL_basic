10 home
20 pen 10
30 r=384
40 clear
50 color 255,128,64
60 for i=1 to 100
70 move 512,384
80 a=i*3.1415927/50
90 r=r-.004
100 k=i+j
110 if k>100 then k=k-100
120 color 255-k*2, 0, 0
130 line 512+r*cos(a), 384+r*sin(a)
140 next i
150 j=j+1
160 if j=100 then j=0
170 if r>0 then 60
180 sleep 1
190 goto 180
