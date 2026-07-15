# MiniKV

> 从零实现的 C++ 高性能 LSM-Tree 键值存储引擎，对标 LevelDB/RocksDB 核心架构。

[![CI](https://github.com/USERNAME/MiniKV/actions/workflows/ci.yml/badge.svg)](https://github.com/USERNAME/MiniKV/actions)

## 性能指标

(待压测填充)

## 架构图

> 见 docs/arch.png

## 快速开始

`ash
# Docker 部署
cd docker && docker compose up -d

# 连接
python client/cli.py localhost:8888
`

## 从源码构建

`ash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
`

## 项目结构

`
include/minikv/       公共 API 头文件
src/core/             LSM-Tree 核心 (SkipList, WAL, SSTable, Compaction)
src/network/           epoll 网络层
src/utils/             工具库 (coding, crc32, hash, LRU, thread_pool)
tests/                 Google Test 单元测试
benches/               基准测试
client/                Python 客户端
docker/                Docker 部署
`

## 设计文档

见 [docs/design.md](docs/design.md)

## 技术栈

C++17, CMake, Epoll, Google Test, Python asyncio

## License

MIT
