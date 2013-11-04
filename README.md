# DoubanFM 客户端

[![Build Status](https://travis-ci.org/zonyitoo/doubanfm-qt.png?branch=master)](https://travis-ci.org/zonyitoo/doubanfm-qt)

使用Qt5编写的DoubanFM客户端

[API接口文档](https://github.com/zonyitoo/doubanfm-qt/wiki/%E8%B1%86%E7%93%A3FM-API)

## 依赖
* libqt5network5 libqt5gui5 libqt5core5 libqt5multimedia5 qtmultimedia5-dev
* gstreamer0.10-fluendo-mp3
* libpulse0 libpulse-dev
* libqt5dbus5

不同平台相应的包名不一样，基本需求是Qt5的基础包，Qt5 Multimedia模块，Qt5 DBus模块，mp3解码器

## 截图

![截图](https://gitcafe.com/zonyitoo/doubanfm-qt/raw/master/screenshot.png)

## 安装方法

在Ubuntu 13.04 x86\_64、13.10 x86\_64及Archlinux上测试通过

```bash
qmake doubanfm-qt.pro
make
```

得到`doubanfm-qt`后直接运行即可

或运行`install.sh`脚本来安装到系统中（For Linux Only），安装好后在桌面系中的的启动器中找到`QDoubanFM`运行即可

## 快捷键
`Ctrl` + `W` 隐藏窗口，最小化到托盘

`Ctrl` + `Q` 退出

`Space` 暂停

`S` 跳过

`D` 不再播放

`F` 标记/取消标记红心

## TODO
* <del>基本播放功能</del>
* <del>频道选择</del>
* <del>用户登录</del>
* <del>快捷键</del>
* <del>动画</del>
* i18n支持
* <del>Linux的播放提示 + DBus</del>
* <del>后台播放 + 托盘提示</del>
* <del>歌词</del>

## BUGS
* <del>在长时间暂停后重启播放会崩溃</del>
* <del>在网络不好时卡住会崩溃</del>
* <del>动画有Bug，若打开了频道界面然后鼠标离开，则会让控制面板滑动位置出错</del>

## LICENSE
本项目基于MIT协议发布

MIT: [http://rem.mit-license.org](http://rem.mit-license.org)

