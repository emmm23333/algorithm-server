# algorithmservice

## 目录结构
* bin 程序生成目录
* docs 需求文档、设计文档等
* externals
    * ustd 通用模块，如日志、线程池、定时器等
        * build 各平台编译脚本
        * include 通用模块头文件
        * lib 通用模块生成的静态库
        * source 通用模块实现
    * lib 程序依赖库
    * objs 编译生成的中间文件
    * source 
        * algorithmservice 算法服务实现
        * common 算法服务工具函数
        * protocol 通信协议
    * thirdparty 第三方依赖

## 配置文件
## 编译方法
```
docker pull alpine:3.9.4
docker run -it --rm -v ${PWD}:/home/algorithm -w /home/algorithm/source/algorithmservice alpine:3.9.4 sh
sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/' /etc/apk/repositories
echo "http://mirrors.aliyun.com/alpine/edge/testing" >> /etc/apk/repositories
apk update
 
apk add --no-cache make libexecinfo-dev util-linux-dev g++ ca-certificates tzdata boost-thread boost-system libstdc++ boost-filesystem freetype libtbb g++ apr-util libcurl mesa-osmesa mesa-gl glu libunwind openssl-dev libxslt docbook-xsl boost boost-dev libpng curl curl-dev libcurl
make clean && make -j8
```
## 运行方法
```
docker pull alpine:3.9.4
docker run -it --rm -v ${PWD}:/home/algorithm -w /home/algorithm/bin alpine:3.9.4 sh
sed -i 's/dl-cdn.alpinelinux.org/mirrors.aliyun.com/' /etc/apk/repositories
echo "http://mirrors.aliyun.com/alpine/edge/testing" >> /etc/apk/repositories
apk update
apk add --no-cache zip libunwind mesa-osmesa mesa-gl boost-thread boost-system boost-filesystem libtbb libcurl glu libpng libgomp mpfr3 gmp libuuid ca-certificates tzdata
cp unzip /usr/bin
./algorithmservice -c algorithmservice.conf
```