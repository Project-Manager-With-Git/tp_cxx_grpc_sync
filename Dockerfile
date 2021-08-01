FROM hsz1273327/vscode_cxx_env:alpine3.13-gcc10-conan1.39.0-vcpkgmaster
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories
RUN apk update 
COPY pip.conf /etc/pip.conf
ENV X_VCPKG_ASSET_SOURCES=x-azurl,http://106.15.181.5/