# CTP-in-cpp
  
## 2020/05/05 09:54(第一次更新)
用C++调用CTP行情接口其实并不难，只要能理解api和spi两类函数分别的作用就行了。但是实现SABR策略就有点头疼了，python给我们带来了太多的便利，优化vega和rho的过程调用scipy的optimize模块一个函数就搞定了，虽然C++也有类似的nlopt库，但是使用起来还是遇到了不少麻烦。我把实现过程中用到的参考资料都列在下面，未来要加强数值分析和优化理论方面的学习……有空就来研究交易接口，希望下周结束前能搞定……  
  
__CTP行情接口调用结果__
  
![CTP行情接口调用结果](https://github.com/yuba316/CTP-in-cpp/blob/master/%E5%9B%BE%E7%89%87/CTP%E8%A1%8C%E6%83%85%E6%8E%A5%E5%8F%A3.png)
  
__SABR参数vega和rho的优化过程__
  
![SABR参数vega和rho的优化过程](https://github.com/yuba316/CTP-in-cpp/blob/master/%E5%9B%BE%E7%89%87/SABR%E6%AF%8F%E6%97%A5%E6%9C%80%E4%BC%98%E5%8C%96vega%E3%80%81rho%E7%B3%BB%E6%95%B0.png)
  
参考文章：
- [C++连接CTP接口实现简单量化交易（行情、交易、k线、策略）](https://blog.csdn.net/u012234115/article/details/70195889)
- [三次样条插值算法](https://www.cnblogs.com/yabin/p/6426849.html)
- [Nlopt优化包在Windows上的安装配置及C/C++应用案例](https://blog.csdn.net/u014571489/article/details/79344676/)
