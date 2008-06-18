10 dim s(8)
20 data 0, 2, 4, 5, 7, 9, 11, 12
30 for i=1 to 8: read s(i) : next i
40 kb=60
41 kn=kb
45 t=0
50 rem ***************** main loop
55 t=t+1
56 if t=9 then t=1:kb=kn
60 tone 1, freq(note(kb+s(t))), dur(.03)
70 sleep .08
80 k=keycode
90 if k>=97 and k<=122 then kn = k-97+47
100 goto 50
