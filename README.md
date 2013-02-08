# DoubanFM 客户端
使用Qt4编写的DoubanFM客户端

## 依赖
* libqt4-network libqt4-gui libqt4-core
* libphonon
* libqjson

## 截图

![截图](https://gitcafe.com/zonyitoo/doubanfm-qt/raw/master/screenshot.png)

## INSTALL

```bash
lrelease lang/*.ts
qmake doubanfm-qt.pro
make
```

得到`doubanfm-qt`后直接运行即可，或

```
mkdir -p /usr/share/QDoubanFM
cp doubanfm-qt /usr/share/QDoubanFM
rm -f /usr/bin/doubanfm-qt
ln -s /usr/share/QDoubanFM/doubanfm-qt /usr/bin/doubanfm-qt
cp QDoubanFM.desktop /usr/share/applications
cp QDoubanFM.png /usr/share/pixmaps
```

或运行`install.sh`脚本同样可以

## TODO
* <del>基本播放功能</del>
* <del>频道选择</del>
* <del>用户登录</del>
* <del>快捷键</del> 在主界面按箭头右是下一首，空格暂停或播放..其他自探索哦哈
* <del>动画</del>
* <del>i18n支持</del>
* Linux的播放提示 + DBus
* 后台播放 + 托盘提示 (Ubuntu下是Indicator)
* 歌词

## BUGS
* <del>在长时间暂停后重启播放会崩溃</del>
* 在网络不好时卡住会崩溃
* <del>动画有Bug，若打开了频道界面然后鼠标厉害，则会让控制面板滑动位置出错</del>

## LICENSE
本项目基于MIT协议发布

MIT: [http://rem.mit-license.org](http://rem.mit-license.org)
