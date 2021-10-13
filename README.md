# Scanner

这是我的毕业设计

## Part $\Iota$

利用红色一字激光的主动扫描

### 设备

转盘——只会不停地转（淘宝随便找的）

摄像头——海康威视E14a

棋盘格——7X5的Charuco

### 建模过程

#### 数据采集

`./resource/video/Calibration.mp4`，是用于进行摄像头以及激光进行标定的

<img src="E:\Engine\README.assets\image-20211013180406391.png" alt="image-20211013180406391" style="zoom: 33%;" />

`./resource/video/ObjectScan.mp4`，是用于进行物体的重建的

<img src="E:\Engine\README.assets\image-20211013180437501.png" alt="image-20211013180437501" style="zoom:33%;" />

#### 摄像头标定

标定这方面，利用棋盘格进行标定已经是比较成熟的了，这个过程当中。需要针对视频每一帧的画面进行选取。首先对于每个画面进行角点的查找，如果可以找到正确的棋盘格，则进行保存，如果找不到就跳过。

这里为了方便后面进行处理，将分割的结果按照图片进行了保存。这时候我们就可以得到相机的内参。

校准之后的值如下：

```xml
<?xml version="1.0"?>
<opencv_storage>
<camera_matrix type_id="opencv-matrix">
  <rows>3</rows>
  <cols>3</cols>
  <dt>d</dt>
  <data>
    1.1454118037202288e+03 0. 9.5950000000000000e+02 0.
    1.1440670673487782e+03 5.3950000000000000e+02 0. 0. 1.</data></camera_matrix>
<distortion_coefficients type_id="opencv-matrix">
  <rows>1</rows>
  <cols>5</cols>
  <dt>d</dt>
  <data>
    2.0051692851426905e-01 -3.2034378798724655e-01 0. 0.
    9.9437015188941894e-02</data></distortion_coefficients>
</opencv_storage>

```

可以看出，这是一个比较经典的1920*1080的摄像头，畸变的程度也是比较小的。

这时候我们可以得到如下的方程

$P_{camera}$表示一个点在摄像机坐标系下的位置，那么其像素位置$P_{pixel}=(p_x, p_y)$

$P^\prime=\left[
    \begin{matrix}
        x^\prime \\ 
        y^\prime \\ 
        z^\prime
    \end{matrix} 
  \right] = Intrinsic \times P_{camera}$

其中$Intrinsic = \left[
    \begin{matrix}
        1.1454118037202288e03 & 0. & 9.5950000000000000e02 \\0. & 1.1440670673487782e03 & 5.3950000000000000e02 \\ 0. & 0. & 1.
    \end{matrix} 
  \right]$

得到的结果即$p_x = x^\prime/z^\prime, p_y=y^\prime/z^\prime$

这是利用齐次方程用相似的方式处理了深度。

#### 求取激光的位置

求取激光的位置，首先我们可以将棋盘格内认为其是激光的像素位置求出来，然后根据四个周围的棋盘格角点的位置，使用双线性插值求出激光在摄像机坐标系的位置。

判断一个点是否是在一个凸多边形内，可以使用叉积进行计算。

然后我们将这些点保存下来。

#### 定标激光所在平面

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

根据$a\bar{x}+b\bar{y}+c\bar{z}=d$求出$d$的值即可。

#### 计算重建视频当中激光的位置

对于每一帧画面，我们可以通过aruco得到一些位置点。

然后我们可以通过`solvePnP`来得到相机的外参，即世界坐标系和相机坐标系当中的转换方式。

$P_{camera}=RP_{world}+T$

$R$表示旋转，$T$表示平移。这里$R$被表示成了一个向量，我们可以通过$Rodrigues$变换将其变为一个旋转矩阵。

然后我们可以将每一个像素点看作一个射线，然后求解和红外平面的交点即可。

### 建模结果

<img src="E:\Engine\README.assets\image-20211013215620498.png" alt="image-20211013215620498" style="zoom: 67%;" />

中间有一部分是因为没有红外线扫过造成的。

### 该方法 Cons,Pros

#### ProS

相对来说比较容易实现，基本上也涵盖了所有这方面需要的基本理论，比较适合新手入门。还有一些标定、aruco的使用。还是比较有趣的。

#### ConS

自动旋转的转盘导致了只能通过视频来处理，不能逐帧处理

由于红色激光亮度比较高，实际上在视频当中变为了白色，很难通过$R-(B+G)/2$找最大点来找到红色激光的中心。

扫描的精度还是比较差的，看起来误差可能有好几毫米，比较一般。

说到底最终的原因还是因为对于激光的定位不好，定位的结果不是一条直线，更像是一个矩阵，把这个矩阵所有的点都用来处理了，结果导致边界比较模糊。