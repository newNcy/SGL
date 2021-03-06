
跟3d数学书一样
坐标系使用左手系，也就是xyz分别对应右上前
向量是行向量，毕竟mvp符合直觉
----------2021-3-7--------------
现在用右手系了，z朝前,因为发现用的两个模型z都朝前


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

-----------------2021-3-6------------------
Xp/Xe = -n/Ze
Xp = -nXe/Ze                            [1]
Yp = -nYe/Ze                            [2]

Xn = Xp/(r-l) * 2 + B
当点处于边缘也就是x = r时，Xn = 1
1 = 2r/(r-l) + B
B   = 1 - 2r/(r-l)
    = ((r-l)-2r)/(r-l)
    = - (r+l)/(r-l)
Xn  = 2Xp /(r-l) - (r+l)/(r-l)         [3]
Yn  = 2Yp /(t-b) - (t+b)/(t-b)         [4]

1.由于远小近大的关系，x y在ndc的值是依赖于观察空间的Ze的，也就是离得越远就越小。
2.在使用变换后坐标处在齐次空间中，裁剪之后需要除以它的W分量得到ndc坐标，也就是透视除法，因为把Ze放到Wc中去了，所以xy的关系式要变成/Ze类型的，在除以w之后就完成Xe->Xn的操作了

[1]代入[3]
Xn  = 2(-nXe/Ze)/(r-l) -(r+l)/(r-l)
    = 2(-nXe/(r-l))/Ze - (r+l)Ze/(r-l)/Ze
    = (2(-nXe/(r-l)-(r+l)Ze/(r-l))/Ze

Xc  = -2n/(r-l) * Xe - (r+l)/(r-l) * Ze     [5]
Yc  = -2n/(t-b) * Ye - (t+b)/(t-b) * Ze     [6]
Wc  = Ze                                    [7]

[5],[6],[7] 写成矩阵形式

                            [-2n/(r-l)          0         __        0 ]
                            [   0           -2n/(t-b)     __        0 ]
                      x     [(r+l)/(r-l)   (t+b)/(t-b)    __        1 ]
                            [   0              0          __        0 ]
                [Xe Ye Ze 1]    [Xc            Yc         Zc        Wc]
因为投影后Zp都是-n了，所以没法像XY一样，Zn是把[-n,-f] 上的点映射到[-1, 1]上，不过因为要除以Wc也就是Ze，它最后的格式也要是/Ze形式的,线性关系如下
Zn = Zc/Wc = (AZe + B)/Ze [8]
当Zn = 1时，Ze=-f
当Zn = -1时，Ze=-n

1 = (-fA + B)/-f            [9]
-1 = (-nA + B)/-n           [10]

[9]->
B = fA-f                [11]
[11]代入[10]
-1 = (-nA + fA-f))/-n
n = -nA + fA - f
n+f = -nA + fA
A = (n+f)/(f-n)         [12]
[12]代入[11]
B   = f(n+f)/(f-n) - f
    = f((n+f)/(f-n) - 1)
    = f ( (n+f)/(f-n) - (f-n)/(f-n) )
    = f (n+f-f+n)/(f-n)
    = f2n/(f-n)

最终表达式为
Zn = ((n+f)/(f-n) * Ze + 2fn/(f-n))/Ze       [13]
Zc = (n+f)/(f-n) * Ze + 2fn/(f-n)
把[14]加进上面的矩阵中,得到投影矩阵

                            [-2n/(r-l)          0          0         0 ]
                            [   0           -2n/(t-b)      0         0 ]
                      x     [(r+l)/(r-l)   (t+b)/(t-b)  (n+f)/(f-n)  1 ]
                            [   0              0        2fn/(f-n)    0 ]
                [Xe Ye Ze 1]  [Xc            Yc          Zc        Wc]
通常来说 r = -l, 所以可简化成
[-2n/(r-l)          0         0        0 ]
[   0           -2n/(t-b)     0        0 ]
[   0              0      (n+f)/(f-n)  1 ]
[   0              0        2fn/(f-n)  0 ]

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
