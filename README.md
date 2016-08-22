车牌字符分割(License Plate Character Segmentation)
-------
该project的基于OpenCV 3.1.0，在Ubuntu 16.04环境中编写与测试。project完成了单排或双排字符车牌分割的任务。

Example
---
* 单排字符车牌分割

   <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/0000.jpg" alt="0000.jpg" width="160" height="60"> <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/arrow1.png" alt="arrow1.png" width="120" height="60"> <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/results/0000.jpg" alt="0000.jpg" width="160" height="60">

* 双排字符车牌分割

   <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/0308.jpg" alt="0308.jpg" width="160" height="120"> <img src="https://github.com/Chaway/LP_character_segmentation/blob/master/arrow2.png" alt="arrow2.png" width="120" height="120"><img src="https://github.com/Chaway/LP_character_segmentation/blob/master/results/0308.jpg" alt="0308.jpg" width="160" height="120">


Ubuntu 16.04下OpenCV环境配置
------
OpenCV配置的教程网上有许多，这里不再详细介绍，可以参考以下链接
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
配置完 ***OpenCV*** 环境后，需要对程序进行编译。可以借助 ***CMake*** 工具自动生成 ***Makefile***，再使用 ***make*** 命令进行编译链接。打开终端后，切换至程序目录
```
cd LP_character_segmentation/
```
输入下列命令即可编译完成，**编译成功后会在目录下生成文件名为 segment 的二进制可执行文件**
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

  ***bash*** 脚本 ***run.sh*** 可以对目录下的所有 ****.jpg*** 文件进行分割（若要分割其它格式的图像，只需将 ***run.sh*** 中的 “jpg” 改成对应的格式后缀），__批量分割时可以将 *segment.cpp* 中显示分割结果的部分代码注释掉__

```
./run.sh
```
分割结果保存在当目录下的 ***results*** 目录中。






