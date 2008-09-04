10 sx = 560
20 sy = 307
30 cls
40 if keycode = 32 then sx = mousex:sy=mousey
50 x = -1.0 + 2.0*mousex/xsize
60 y = 1.0 - 2.0*mousey/ysize
70 px = x
80 py = y
90 x = -1 + 2.0*sx/xsize
100 y = 1 - 2*sy/ysize
105 x = px:y=py:px=0:py=0
110 for i=1 to 1024
120 rem See The Mandelbrot Set
130 temp = px*px - py*py + x
140 py = 2*px*py + y
150 px = temp
160 tx = (px+1)*xsize/2
170 ty = (1-py)*ysize/2
180 if tx<0 or tx>xsize-1 or ty<0 or ty>ysize-1 then 210
190 disc tx,ty,3
200 next i
210 rem
220 update
230 goto 30
