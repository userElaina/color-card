# color-card

从图片中提取色卡

### About

`Python` 太慢了,改用 `C++`.

先用 `ffmpeg` 将图片转为位图再处理.
要不然还得用 `opencv`,没必要.

将图片先缩小再处理就很不必要了,因为 `C++` 跑得比香港记者还快.

### Function

#### Basic

`#include` 头文件后,可手动调整变量 `OUTPUT` 为以下常量的或.

`OUTPUT_DEBUG` 是否控制台输出debug信息

`OUTPUT_PROGRESS` 是否控制台输出当前执行进度(如 `"Loading..."` ).

`OUTPUT_LIST` 是否控制台输出色卡信息(格式 `"%d %06X %d",level,color,pixels` ).

`OUTPUT_HTML` 是否保存如下样子的 `html` .

![image](./log/p1)

`OUTPUT_PIC` 是否保存 `line` 函数处理后的图片.

#### `crowd`

将像素分类,按照比例提取色卡.

```cpp
void crowd(std::string pth,double limit=0.01,int need=Q_MAX);
```

`pth` 文件路径.

`limit` 色系占比小于 *limit* 的颜色将被忽略.

`need` 获得一个 *need* 种颜色的色卡.

html中的 `Level n` 意味着将颜色分为 `1<<(n*3)` 类计数.

#### `naive`

将马赛克,根据像素点提取色卡.

```cpp
void naive(std::string pth,int needx=4,int needy=4);
```

`pth` 文件路径.

`needx needy` 获得一个 *needx\*needy* 种颜色的色卡.

html中的 `Level` 无意义.

#### `line`

将图片勾线/提取线稿+调色.

```cpp
void line(std::string pth,int dis=96,int onlyline=0,int color=-1)
```

`pth` 文件路径.

`dis` 数字越大,勾线越少.当 `dis>=765` 时,一定不会有任何勾线.

`onlyline` 是否只保留线稿.

`color` 负值表示不调色,正值表示调色使用的标准颜色.

![image](./log/p2)
原图(左上), 勾线(右上), 线稿(左下), 勾线+调色(右下)

### 由来

![image](./log/p0)

### Old Version

##### Python

返回值是 `[[b,g,r,value],...]`.

### To Do

将图片划分为不规则的颜色相近的区域,并生成色卡.

命令行工具.
