10 r=32
20 bx=xsize*9/10
30 by=ysize/2
40 ox=bx
50 oy=by
60 dx = -r*2/3
70 if dx>-r/2 then 60
80 dy=rnd(r*2+1)-r
90 if dx*dx + dy*dy < r*r/2 then 60
100 b=10: rem border size
110 ph=75: REM paddle height
120 cls
130 y=mousey
140 if y<ph then y=ph
150 if y>ysize-ph then y=ysize-ph
160 box 50,y,15,ph
170 bx = bx + dx
180 if bx<-b then 20
190 gosub 320
200 if bx>xsize-b and dx > 0 then dx = -dx
210 by = by + dy
220 if by<b and dy < 0 then dy = -dy
230 if by>ysize-b and dy > 0 then dy = -dy
240 move ox,oy
250 pen 20
260 line bx,by
270 ox=bx
280 oy=by
290 update
300 sleep .025
310 goto 120
320 if bx>65 then return
330 if bx<35 then return
340 if by<y-ph then return
350 if by>y+ph then return
360 if dx>0 then return
370 dx = -dx
380 dy = (by-y)*.4
390 return
