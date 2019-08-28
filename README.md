我觉得大家可能也不会用xftp, 会用你们也搞不定写入权限，所以还是用git方便点。

安装vsftpd，sudo apt install vsftpd -y
然后去 /etc/vsftpd.conf 里把write_enable前边的#删掉
重启vsftpd，不会直接重启整个虚拟机，如果之前做过这部，以后就不用再做了。

再装个git，sudo apt install git -y
然后把代码下载下来，git clone [https://gitlab.com/cnztos/IA-FTPclient](https://gitlab.com/cnztos/IA-FTPclient)
远程仓库更新之后  git pull   一下，把本地的也更新了。

然后cd到  IA-FTPclient， cd IA-FTPclient
我的电脑是64位的，我编译的你们可能用不了，自已编译下
~~安装隐藏密码的依赖包 sudo apt-get install libncurses5-dev~~
gcc client.c util.c function.c updown.c -o client -std=c99

运行 ./client 127.0.0.1  ，总之就是  ./接编译出来的文件名，空格 ftp服务器地址

然后用户名，密码登上去

涉及相对路径和绝对路径的概念，去查查，不然搞不懂文件在哪，那天我就有点没理清楚。
p1,p2可以是相对路径，也可以是绝对路径。不过用之前你们最好搞明白这2个路径，就是，我的本地路径在哪？我的ftp的远程路径在哪？

操作命令
pwd                 显示当前路径
ls                     列出当前目录里的文件
ls path            列出后边接的文件夹里的文件
rename p1 p2          把p1名字改成p2
put p1 p2         把本地文件p1，放到服务器的p2上
get p1 p2         把服务器文件p1，放在本地p2
rest p1 p2        断点续传，把没传完的服务器文件p1，放在本地没传完的p2，附加功能我做的错误处理不够，你们用之前先确定，p2比p1文件小。
cd path            切换到path
mkdir path       创建文件夹
del path           删除文件
rm path            删除文件夹
binary              切换binary模式
ascii                 切换ascii模式
pasv                传数据使用pasv模式
port                 传数据使用active模式
speed 100      设定速度，单位是每秒传多少bytes，默认是2000，只在上传时有效。  别传太大的文件，不然传不完。给老师演示限速的时候，把速度调下来，给个2,3或4的就行。
quit                 退出


openssh-server 允许root密码登录：
我那天敲的  sudo -i  算是个人习惯吧，因为我喜欢用root，你们正常用不需要的，用student照样运行。但是如果你们用着烦的话，那就干脆把student扔了，只用root吧。
给root设个密码，1234什么的    sudo passwd root
sudo vim /etc/ssh/sshd_config
找到   PermitRootLogin prohibit-password   这行，改成    PermitRootLogin yes
重启虚拟机
在xshell里右键，属性，用户身份验证，把用户名改成root，输root的密码，确定
再连就直接是root了，可以彻底扔掉sudo，也不会遇到权限问题。

Q:rest的时候为什么我的文件换行了？
A:真不是我写的有问题，是vim编辑的时候，vim会在你保存的时候给文件末尾换一行。而且我看了，linux下常用的编辑器好像都有这个特性，所以要么像我当初测试时候一样，在用xftp打开那个文件再编辑。要么参考这篇，在你编辑的时候禁止vim末尾自动换行，[http://www.51testing.com/html/93/218893-864543.html](http://www.51testing.com/html/93/218893-864543.html)


Q: 为什么代码里这么多goto？
A: 因为很方便，linux内核里也有好多goto

Q: 怎么会有自己写的头文件
A: 写一起太多，乱的找不到

Q: 一些函数的用涂，trim strtok, memset等
A: 去查查，知道能干什么就行

Q: 速度控制代码
A: 在updown.c 140-148行
