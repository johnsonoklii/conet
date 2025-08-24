# conet

## 项目简介
conet 是一个基于 C++ 的高性能网络库，专注于提供轻量级的协程支持和高效的网络通信能力。项目包含以下核心模块：

1. **协程支持**：提供协程调度、上下文切换和钩子功能。
2. **网络通信**：实现 TCP 服务器/客户端、事件循环和定时器功能。
3. **日志系统**：支持多目标输出（文件、控制台）和格式化日志。
4. **工具模块**：包括线程管理、时间戳和同步工具。

## 功能模块

### 基础模块 (`src/conet/base/`)
- **协程 (`coroutine/`)**
  - 协程上下文管理 (`coctx.h`)
  - 协程核心功能 (`coroutine.h`)
  - 协程钩子 (`hook.h`)
- **日志 (`log/`)**
  - 日志记录器 (`logger.h`)
  - 日志输出目标 (`appender.h`)
  - 日志格式化 (`formatter.h`)
- **工具 (`util/`)**
  - 时间戳 (`timestamp.h`)
  - 线程管理 (`thread.h`)
  - 同步工具 (`waitgroup.h`)

### 网络模块 (`src/conet/net/`)
- **核心组件**
  - 事件循环 (`eventLoop.h`)
  - 事件通道 (`channel.h`)
  - 定时器 (`timer.h`)
- **TCP 相关**
  - TCP 服务器 (`tcp_server.h`)
  - TCP 连接管理 (`tcp_connection.h`)
  - 数据缓冲区 (`buffer.h`)

### 测试模块 (`test/`)
- **网络测试 (`net/`)**
  - 协程 TCP 服务器测试 (`test_co_server.cc`)
  - 定时器测试 (`test_timer.cc`)
- **基础测试 (`base/`)**
  - 协程和日志功能测试

## 构建与运行

### 依赖
- CMake 3.0 或更高版本
- 支持 C++ 的编译器

### 构建步骤
1. 克隆项目：
   ```bash
   git clone <项目地址>
   cd conet
   ```
2. 使用 `build.sh` 脚本构建：
   ```bash
   ./build.sh [--debug|--release]
   ```
   - `--debug`: 构建调试版本
   - `--release`: 构建发布版本（默认）

3. 可选：安装
   ```bash
   ./build.sh --i
   ```

## 示例与测试
- 示例代码位于 `example/` 目录。
- 测试代码位于 `test/` 目录，可通过 CMake 选项启用测试构建。

## 贡献
欢迎提交 Issue 或 Pull Request 改进项目。