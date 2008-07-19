10 cls
20 x = -1.0 + 2.0*mousex/xsize
30 y = 1.0 - 2.0*mousey/ysize
40 px = 0
50 py = 0
60 for i=1 to 1024
70 rem See The Mandelbrot Set
80 temp = px*px - py*py + x
90 py = 2*px*py + y
100 px = temp
110 tx = (px+1)*xsize/2
120 ty = (1-py)*ysize/2
130 if tx<0 or tx>xsize-1 or ty<0 or ty>ysize-1 then 170
140 move tx, ty
150 spot
160 next i
170 rem
180 update
190 goto 10
