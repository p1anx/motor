## 在HAL库基础上，建立自己库
- 自建库内容放在`src`文件内，同时需要在`platformio.ini`文件中添加代码路经
- 在`Core/Src`中的`main.c`中加入`mymain()`函数

## for python
`serial_read.py` is read the data from `uart`, and creates the `.csv` file for `Bx By Bz`
`decode_csv.py` can convert the `bit stream` to `string` or `char`