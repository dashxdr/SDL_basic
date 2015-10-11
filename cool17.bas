10 cls
20 sprite=loadtexture("sprite.png")
30 print sprite
40 for j=1 to 100
50 for i=1 to 100
60 drawtexture sprite, rnd(xsize), rnd(ysize)
70 next i
80 update
90 sleep .02
100 next j
110 drawtexture sprite, mousex, mousey
120 update
130 sleep .02
140 goto 110
