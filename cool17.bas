10 cls
20 icon=loadtexture("icon.png")
30 print icon
40 for j=1 to 100
50 for i=1 to 100
60 drawtexture icon, rnd(xsize), rnd(ysize)
70 next i
80 update
90 next j
