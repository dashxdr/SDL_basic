10 n=0
20 dim p(10000)
30 for i=2 to 10000
40 if n=0 then 100
50 s=sqr(i)
60 for j=1 to n
70 if i/p(j) = int(i/p(j)) then 140
80 if p(j)>s then 100
90 next j
100 n=n+1
110 p(n)=i
120 print i;
130 if n/20 = int(n/20) then print
140 next i
150 print
