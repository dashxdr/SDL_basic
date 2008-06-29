10 pen 15
20 ro=ysize*.4
30 ri=ro/3
40 cls
50 color 255,200,0
60 wedge xsize/2, ysize/2, ri, ro, a+30-20, 40
70 wedge xsize/2, ysize/2, ri, ro, a+150-20, 40
80 wedge xsize/2, ysize/2, ri, ro, a+270-20, 40
90 color 255,0,0
100 arc xsize/2, ysize/2, ro, a2+30-20, 40
110 arc xsize/2, ysize/2, ro, a2+150-20, 40
120 arc xsize/2, ysize/2, ro, a2+270-20, 40
130 update
140 sleep .02
150 a=a+20*mousex/xsize-10
160 if a>=360 then a=a-360
170 if a<0 then a=a+360
180 a2=a2-20*mousey/ysize+10
190 if a2<0 then a2=a2+360
200 if a2>=360 then a2=a2-360
210 goto 40
