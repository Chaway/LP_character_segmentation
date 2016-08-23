车牌字符分割(License Plate Character Segmentation)
-------
该 **project** 实现了单排或双排字符车牌分割的功能。  **project** 基于 **OpenCV 3.1.0**，在 **Ubuntu 16.04** 环境中完成编写与测试，所以在使用程序时需要先配置好 **OpenCV 3.1.0**，配置方法可以参考下面列出的链接。此外，程序在 **Linux** 下详细的使用方法也可以参考下面的教程。


Example
---
* 单排字符车牌分割

   <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/0000.jpg" alt="0000.jpg" width="160" height="60"> <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/arrows/arrow1.png" alt="arrow1.png" width="120" height="60"> <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/results/0000.jpg" alt="0000.jpg" width="160" height="60">

* 双排字符车牌分割

   <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/0308.jpg" alt="0308.jpg" width="160" height="120"> <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/arrows/arrow2.png" alt="arrow2.png" width="120" height="120"><img src="https://github.com/Chaway/LP_character_segmentation/blob/master/results/0308.jpg" alt="0308.jpg" width="160" height="120">


Ubuntu 16.04 下 OpenCV 环境配置
------
**OpenCV** 配置的教程网上有许多，这里不再详细介绍，可以参考以下链接
- [Tutorial linux install](http://docs.opencv.org/3.1.0/d7/d9f/tutorial_linux_install.html)
- [Install opencv 3.0 and python 2.7 on ubuntu](http://www.pyimagesearch.com/2015/06/22/install-opencv-3-0-and-python-2-7-on-ubuntu/)
- [OpenCV on GitHub](https://github.com/opencv/opencv)



下载
------
在终端中执行下列命令复制 ***repository*** 到本地，或者可以直接下载 ***.zip*** 格式的压缩文件
```
git clone https://github.com/Chaway/LP_character_segmentation.git
```


程序编译
-------
配置完 ***OpenCV*** 环境后，需要对程序进行编译。可以借助 ***CMake*** 工具自动生成 ***Makefile***，再使用 ***make*** 命令进行编译链接。在终端中进入程序所在目录
```
cd LP_character_segmentation/
```
然后在终端输入下列命令即可编译完成，**编译成功后会在目录下生成文件名为 *segment* 的二进制可执行文件**
```
cmake ./;make
```
若要添加或删减程序文件，编译前需要相应地修改目录下的 ***CMakeLists.txt***


运行
-----

- **单次分割**

  在终端执行程序，***filename ***是图像文件的文件名（程序文件和待分割图像都在当前目录下，否则需要指明路径）
  
```
./segment filename
```

- **批量分割**

  ***bash*** 脚本 ***run.sh*** 可以对 ***image*** 目录下的所有车牌图像进行分割，批量分割时可以将 ***segment.cpp*** 中显示分割结果的部分代码注释掉

```
./run.sh
```
分割结果保存在当前目录下的 ***results*** 目录中。

原理
------
程序用到的车牌字符分割原理可以参考 ***pdf*** 目录下的文档

