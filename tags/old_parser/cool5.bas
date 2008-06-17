10 home
20 cx = xsize/2
30 cy = ysize/2
40 pen 10
50 r=384
60 cls
70 color 255,128,64
80 for i=1 to 100
90 move cx,cy
100 a=i*3.1415927/50
110 r=r-.004
120 k=i+j
130 if k>100 then k=k-100
140 color 255-k*2, 0, 0
150 line cx+r*cos(a), cy+r*sin(a)
160 next i
165 update
170 j=j+1
180 if j=100 then j=0
190 if r>0 then 80
200 sleep 1
210 goto 200
