# color-card

从图片中提取色卡

### About

`Python` 太慢了,改用 `C++`.

先用 `ffmpeg` 将图片转为位图再处理.
要不然还得用 `opencv`,没必要.

将图片先缩小再处理就很不必要了,因为 `C++` 跑得比香港记者还快.

### Function

#### Input

```cpp
void crowd(char*pth,double limit=0.01,int need=Q_MAX);
```

`pth` 文件路径.

`limit` 色系占比小于它的颜色将被忽略.

`need` 色卡中需要多少颜色.

#### Output

`#include` 头文件后,可手动调整变量 `LOG`.

`LOG&1` 是否输出当前执行状态信息(如 `"Loading..."` ).

`LOG&2` 是否输出(8个等级的)色卡信息(格式 `"%d %06X %d",level,color,pixels` ).

`LOG&4` 是否输出如下样子的 `html` .

![image](./log/p1)

若调用 `crowd` , `Level n` 意味着将颜色分为 `1<<(n*3)` 类计数.

### 由来

![image](./log/p0)

### Old Version

##### Python

返回值是 `[[b,g,r,value],...]`

### To Do

规则分块生成色卡.

不规则分块生成色卡.

命令行工具.
