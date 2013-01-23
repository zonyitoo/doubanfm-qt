# DoubanFM 客户端
使用Qt4编写的DoubanFM客户端

## 依赖
* libqt4
* libphonon
* libqjson

## 截图

![截图](https://gitcafe.com/zonyitoo/doubanfm-qt/raw/master/screenshot.png)

## INSTALL

```bash
qmake doubanfm-qt.pro
make
```

得到`doubanfm-qt`后直接运行即可，或

```
cp doubanfm-qt /usr/bin
cp QDoubanFM.desktop /usr/share/applications
cp QDoubanFM.png /usr/share/pixmaps
```

或运行`install.sh`脚本同样可以

## TODO
* <del>基本播放功能</del>
* <del>频道选择</del>
* <del>用户登录</del>
* 快捷键

## BUGS
* 在长时间暂停后重启播放会崩溃

## LICENSE
本项目基于MIT协议发布

MIT: [http://rem.mit-license.org](http://rem.mit-license.org)
