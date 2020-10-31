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
这个没啥好说的，推导没仔细看，直接用正交阵求正向变换的逆然后拼一个反向的平移操作

## 齐次裁剪 
>向量点乘 P(1,0,0,1) = 0时表示 x + w = 0也是就 x = -w ,式子大于0表示x > -w 也就是超出裁剪面了，
>然后使用SutherlandHodgemand算法对每条边生成交点，具体四种情况，写代码里了
## 透视纹理纠正
使用重心坐标和扫描线两端距离直接对纹理坐标进行插值得到的纹理坐标都是不对的
因为这两个都是屏幕坐标来计算的，所以它们的线性关系跟真实世界的线性关系不等。
但是除以各自的w之后，就是成线性关系的了

公式推导 http://www.cs.ucr.edu/~craigs/courses/2019-fall-cs-130/lectures/perspective-correct-interpolation.pdf

代码实现
float k = w1/a.position.w + w2/b.position.w + w3/c.position.w;
w1 = w1/a.position.w/k;
w2 = w2/b.position.w/k;
w3 = w3/c.position.w/k;
f.uv = a.uv*w1 + b.uv*w2 + c.uv*w3;
其中w1 w2 w3是计算出来重心坐标
