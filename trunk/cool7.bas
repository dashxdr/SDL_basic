10 cls
20 home
30 a=1
40 r=0
50 k=k2
52 k2=k2+5
53 if k2>511 then k2=k2-512
60 if k<256 then c=k else c=511-k
70 color 0,0,c
80 k=k+50
90 if k>511 then k=k-512
100 disc xsize/2+r*cos(a),ysize/2-r*sin(a),16
110 a=a+1/a
120 r=5*a
130 if r<300 then 60
135 update
136 sleep .02
140 goto 30
