# Scanner

这是我的毕业设计

## $\Iota$

利用红色激光的主动扫描

### Algorithm one

利用SVD分解求出激光所在的平面

已知若干三维点$(x_i, y_i, z_i)$，拟合出平面方程$ax+by+cz=d$

平均坐标点为$(\bar{x}, \bar{y}, \bar{z})$，有$a\bar{x}+b\bar{y}+c\bar{z}=d$

则可得 $a(x_i-\bar{x})+b(y_i-\bar{y})+c(z_i-\bar{z}) = 0$

写成矩阵的形式

$
A=\left[
    \begin{matrix}
        x_1-\bar{x} & y_1-\bar{y} & z_1-\bar{z} \\ 
        x_2-\bar{x} & y_2-\bar{y} & z_2-\bar{z} \\ 
        x_3-\bar{x} & y_3-\bar{y} & z_3-\bar{z} \\ \vdots & \vdots & \vdots \\ 
        x_n-\bar{x} & y_n-\bar{y} & z_n-\bar{z}
    \end{matrix} 
  \right]
$

$
X=\left[
    \begin{matrix}
        a \\
        b \\
        c \\
    \end{matrix}
  \right]
$

则希望最小化 $AX = 0$

$||AX|| = ||UDV^{T}X|| = ||DV^{T}X||$

当$||V^TX||=1$时取到最小值，那么对应的就是最小的特征值

因此求出了$a,b,c$

根据$a\bar{x}+b\bar{y}+c\bar{z}=d$求出D的值即可。


## $\Iota\Iota$

结构光扫描