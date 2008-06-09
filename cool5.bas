3 pen 10
5 r=384
10 clear
20 color 255,128,64
30 for i=1 to 100
40 move 512,384
50 a=i*3.1415927/50
60 r=r-.004
62 k=i+j
63 if k>100 then k=k-100
64 color 255-k*2, 0, 0
70 line 512+r*cos(a), 384+r*sin(a)
80 next i
90 j=j+1
95 if j=100 then j=0
100 if r>0 then 30
110 sleep 1
120 goto 110
