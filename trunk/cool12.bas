10 cls
20 home
30 print
40 input "How many vertices(example: 59)"; vnum
50 input "Step between vertices(example: 19)"; vstep
60 cls
70 print "Number = "; vnum ; "  Step = ";vstep
80 pi2=3.1415927*2
90 da = vstep * pi2/vnum
100 tc = 0
110 a = 0
120 r = ysize/2 - 5
130 move xsize/2 + r*cos(a), ysize/2 - r*sin(a)
140 a = a+da
150 if a>=PI2 then a = a - PI2
160 line xsize/2 + r*cos(a), ysize/2 - r*sin(a)
170 tc = tc + vstep
180 if tc >= vnum then tc = tc - vnum
190 if tc <> 0 then 140
200 goto 20
