# Fluid Simulation #
# 题目 #
基于闫令琪老师GAMES101课程的拓展。</br>编写二维流体动力学模拟程序，以估算Navier-Stokes方程下不可压缩的均匀流体的行为。
# 实现 #
参考了胡渊明老师GAMES201课程欧拉视角部分的讲解。</br>
cmake框架借鉴了GAMES101的P8。</br></br>
采用欧拉视角。</br>
原始NS方程为 $$ \rho \frac{Du}{Dt} = -\nabla p + \nabla · \tau + \rho g $$ </br>
不考虑粘性项，有$$ \frac{Du}{Dt} = -\frac{1}{\rho} \nabla p + g $$ </br>
考虑不可压缩的限制，有 $$ \nabla · u = 0 $$</br></br></br>
我们将其分为几部分:</br>
$$ \frac{Du}{Dt} = 0 (advection)$$
$$ \frac{\delta u}{\delta t} = g (external force)$$ 
$$ \frac{\delta u}{\delta t} = -\frac{1}{\rho} \nabla p (projection)$$</br></br>
对于advection，使用了较易实现的semi-lagrangian与RK2。</br>
对于projection，对原式做有限差分并化简后可以得到$$ \nabla · \nabla p = \frac{\rho}{\Delta t}\nabla · u $$
在我们的二维网格中做离散处理，$$ (\nabla · \nabla p)\_{i,j} = \frac{1}{\delta x^{2}}(-4p\_{i,j} + p\_{i+1,j} + p\_{i-1,j} + p\_{i,j-1} + p\_{i,j+1}) $$
$$ (\frac{\rho}{\Delta t}\nabla · u)\_{i,j} = \frac{\rho}{\Delta t \Delta x}(u^{x}\_{i+1,j} - u^{x}\_{i,j} + u^{y}\_{u,j+1} - u^{y}\_{i,j}) $$</br>
得到一个线性系统，通过advection中求出的u来解p。这里采用了易实现的jacobi迭代解线性系统，迭代次数设置为400。</br>
并加入了一些视觉上的增强效果</br></br>

光栅化部分尚未完善。
# 依赖 #
需预先安装OpenGL, Freetype和RandR。
# 运行命令 #
	$ mkdir build
	$ cd build
	$ cmake . .
	$ make
	$ ./ropesim
# 效果 #
以下动图展示了运行效果：</br></br>
![avatar](https://github.com/seeeagull/Fluid-Simulation/blob/master/demo/ns.gif)</br>
# 参考资料 #
https://en.wikipedia.org/wiki/Navier%E2%80%93Stokes_equations
