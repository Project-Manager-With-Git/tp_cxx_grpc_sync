FROM {{ cross_compiling }}alpine:3.13 as build_img
# 编译grpc可执行程序
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories
RUN apk update && apk --no-cache add gcc g++ git autoconf libtool pkgconfig make cmake && rm -rf /var/cache/apk/*
COPY pbschema /code/pbschema
COPY src/ /code/src/
COPY CMakeLists.txt /code/CMakeLists.txt
RUN cmake -Bbuild .
WORKDIR /code/build
RUN make


# 构造grpc-health-probe
FROM {{ cross_compiling }}golang:1.16-alpine as build_grpc-health-probe
ENV GO111MODULE=on
ENV GOPROXY=https://goproxy.io
# 停用cgo
ENV CGO_ENABLED=0
# 安装grpc-health-probe
RUN go get github.com/grpc-ecosystem/grpc-health-probe


# 使用upx压缩可执行文件
FROM {{ cross_compiling }}alpine:3.11 as upx
WORKDIR /code
# 安装upx
RUN sed -i 's/dl-cdn.alpinelinux.org/mirrors.ustc.edu.cn/g' /etc/apk/repositories
RUN apk update && apk add --no-cache upx && rm -rf /var/cache/apk/*
COPY --from=build_bin /code/tp_cxx_grpc_sync ./tp_cxx_grpc_sync-cxx
RUN upx --best --lzma -o tp_cxx_grpc_sync tp_cxx_grpc_sync-cxx


# 使用压缩过的可执行文件构造镜像
FROM {{ cross_compiling }}alpine:3.13 as build_upximg
COPY --from=build_grpc-health-probe /go/bin/grpc-health-probe .
RUN chmod +x /grpc-health-probe
COPY --from=upx /code/tp_cxx_grpc_sync .
RUN chmod +x /tp_cxx_grpc_sync
EXPOSE 5000
HEALTHCHECK --interval=30s --timeout=30s --start-period=5s --retries=3 CMD [ "/grpc-health-probe","-addr=:5000" ]
ENTRYPOINT [ "/tp_cxx_grpc_sync"]