# DoubanFM 客户端

使用Qt5编写的DoubanFM客户端

如果你喜欢本项目的话，给我买瓶啤酒喝好不;)

<!--支付宝已经下线了这个功能
<a href='http://me.alipay.com/zonyitoo'> <img src='https://img.alipay.com/sys/personalprod/style/mc/btn-index.png' /> </a>
-->

![支付宝](https://tfsimg.alipay.com/images/mobilecodec/T1nwBdXXdlXXXXXXXX)

### 注意

* Ubuntu由于不支持Qt的`QSystemTrayIcon`，直接使用会有Bug，**必须**在生成`Makefile`前在`doubanfm-qt.pro`中删除`DEFINES += WITH_SYSTEM_TRAY_ICON`！

* KDE用户在打开时载入会出现`SIGSEGM`，目前*原因未知*，如果有人知道解决方法请告诉我谢谢！Issues中有提及。

* `Ctrl`+`W`后软件界面消失为**正常行为**，只要重新点图标（启动器）就可以唤醒。若强行Kill会导致此功能失效，需要手动删除`/tmp/QDoubanFM_LocalSocket`，或者会在其他位置，不同发行版可能位置不同。

* Windows及OSX上的有兼容性问题，但功能大致正常。

[API接口文档](https://github.com/zonyitoo/doubanfm-qt/wiki/%E8%B1%86%E7%93%A3FM-API)

## 依赖
* qt5-base
* libqt5network5 libqt5gui5 libqt5core5 libqt5multimedia5 qtmultimedia5-dev
* gstreamer0.10-fluendo-mp3
* libpulse0 libpulse-dev
* libqt5dbus5
* libqt5multimedia5-plugins

不同平台相应的包名不一样，基本需求是Qt5的基础包，Qt5 Multimedia模块，Qt5 DBus模块，mp3解码器

## 截图

![截图](https://gitcafe.com/zonyitoo/doubanfm-qt/raw/master/screenshot.png)

## 安装方法

* 在Archlinux上测试通过

```bash
lupdate doubanfm-qt.pro
lrelease doubanfm-qt.pro
qmake doubanfm-qt.pro
make
```

得到`doubanfm-qt`后直接运行即可

或运行`install.sh`脚本来安装到系统中（For Linux Only），安装好后在桌面系中的的启动器中找到`QDoubanFM`运行即可

* Ubuntu

已经在Ubuntu 13.04 x86\_64至Ubuntu 15.04 x86\_64上测试通过。

在Ubuntu版本低于15.04时，Qt的系统托盘不能正常工作，所以
先使用编辑器打开`doubanfm-qt.pro`，删除

```
DEFINES += WITH_SYSTEM_TRAY_ICON
```

剩余步骤与上面相同。

* openSUSE

[一键安装](https://software.opensuse.org/download.html?project=home%3Aopensuse_zh&package=doubanfm-qt5)

* Windows或OSX

这两个系统比较特殊，建议到官网下载QtCreator打开`doubanfm-qt.pro`选择`Release`编译。

## 快捷键
`Ctrl` + `W` 隐藏窗口，最小化到托盘

`Ctrl` + `Q` 退出

`Space` 暂停

`S` 跳过

`D` 不再播放

`F` 标记/取消标记红心

## TODO
- [x] 基本播放功能
- [x] 频道选择
- [x] 用户登录
- [x] 快捷键
- [x] 动画
- [ ] i18n支持
- [x] Linux的播放提示 + DBus
- [x] 后台播放 + 托盘提示
- [x] 歌词

## KNOWN BUGS
- [x] 在长时间暂停后重启播放会崩溃
- [x] 在网络不好时卡住会崩溃
- [x] 动画有Bug，若打开了频道界面然后鼠标离开，则会让控制面板滑动位置出错
- [x] KDE4.9 上未知原因崩溃 (Fixed in KDE5)
- [ ] Ubuntu不支持`QSystemTrayIcon`导致软件不能正常退出

## LICENSE
本项目基于MIT协议发布

MIT: [http://rem.mit-license.org](http://rem.mit-license.org)

