## 投影矩阵

Xe Ye Ze 经过摄像机矩阵变换的xyz，eye space
Xp Yp Zp 投影后的xyz

n 近面 f远面
根据相似三角形

Xp = n*Xe/Ze
Yp = n*Ye/Ze
Zp = n
=>因为4d向量转到3d空间时，需要除以w分量，让w分量 = z/n时则可以完成上述变换,矩阵为
1 0 0 0
0 1 0 0
0 0 1 1/n
0 0 0 0

Xn = Xp * A + B
A是斜率 A = 2/w
w对称，B = 0

Xn  = Xp* 2/W
	= n*Xe*2/W/Ze

Yn	= n*Ye*2/H/Ze


Zn	=  A*Ze + B    / Ze

Zn = 1时，Ze = f
1 = (A*f + B)   /  f  式子1
-1 = (A*n + B)  /  n 式子2

f = Af + B
-n = An + B
f + n = A(f-n)

A = (f+n)/(f-n)

f = (f+n)*f/(f-n) + B
B = f - (f+n)*f/(f-n)
B = f(1-(f+n)/(f-n))
B = f((f-n-f-n)/(f-n))
B = -2nf/(f-n)

## 观察矩阵
已知有坐标·
	
