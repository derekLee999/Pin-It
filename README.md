<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="docs/assets/wordmark-dark.png">
    <img src="docs/assets/wordmark-light.png" width="360" alt="PinIt — 将任意窗口置顶">
  </picture>
</p>

<p align="center">
  <b>在 Windows 11 和 10 上让任意窗口始终置顶 — 一键搞定，全局快捷键。</b>
</p>

<p align="center">
  <b>🔗 原项目：<a href="https://github.com/Razee4315/Pin-It">github.com/Razee4315/Pin-It</a>（本项目 Fork 自此）</b>
</p>

<p align="center">
  <a href="https://github.com/Razee4315/Pin-It/releases/latest"><img src="https://img.shields.io/github/v/release/Razee4315/Pin-It?style=flat-square" alt="最新版本"></a>
  <a href="https://github.com/Razee4315/Pin-It/releases"><img src="https://img.shields.io/github/downloads/Razee4315/Pin-It/total?style=flat-square" alt="下载量"></a>
  <a href="https://github.com/Razee4315/Pin-It/releases/latest"><img src="https://img.shields.io/badge/platform-Windows%2010%20%7C%2011-0078D6?style=flat-square" alt="平台"></a>
  <a href="https://www.qt.io"><img src="https://img.shields.io/badge/built%20with-C%2B%2B%20%26%20Qt%206-41CD52?style=flat-square" alt="基于 C++ 和 Qt 6 构建"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-Apache--2.0-green?style=flat-square" alt="许可证: Apache 2.0"></a>
</p>

按下 `Win+Ctrl+T`，当前窗口就会置顶显示。拖动滑块调节透明度，可以透视窗口。重启电脑后 PinIt 会自动重新置顶。一个专注于单一功能的轻量工具 — 使用原生 C++ 和 Qt 编写，直接调用 Windows API。

<p align="center">
  <img src="docs/assets/pin-it-demo.gif" width="720" alt="PinIt 演示：使用 Win+Ctrl+T 将记事本窗口置顶，然后调节透明度透视窗口">
  <br>
  <em>置顶窗口、调节透明度、继续工作 — 全部通过一个快捷键完成</em>
</p>

## 下载

**[⬇ 下载最新版本](https://github.com/Razee4315/Pin-It/releases/latest)** — 支持 Windows 10 和 11，完全免费。

| 文件 | 说明 |
|------|------|
| `PinIt_x.y.z_x64-setup.exe` | 安装包（推荐）— 包含开始菜单快捷方式和卸载程序 |
| `PinIt-portable-x64.zip` | 便携版 — 解压即用，无需安装 |

> **注意：** 安装包尚未签名，Windows SmartScreen 可能会显示"Windows 已保护你的电脑"。请点击 **更多信息 → 仍要运行**。PinIt 完全开源（Apache 2.0）— 你可以审计代码或自行构建。

## 功能特性

- **全局快捷键置顶** — `Win+Ctrl+T` 置顶/取消置顶当前窗口，无需点击菜单。
- **窗口透明度** — 使用 `Win+Ctrl+=` / `Win+Ctrl+-` 或滑块调节任意已置顶窗口的透明度。适合参考资料、视频通话或笔记叠加显示。
- **重启后自动恢复** — PinIt 记住你置顶的窗口（及其透明度），重新登录后自动恢复。
- **Windows 11 置顶强化** — Win11 的合成器有时会移除置顶标志，PinIt 会自动重新应用。
- **系统托盘应用** — 关闭窗口后最小化到托盘，不打扰你的工作。支持开机自启。
- **轻量快速** — 原生 C++/Qt 直接调用 Windows API，占用内存极小，响应迅速。

## 快捷键

| 操作 | 默认快捷键 |
|------|-----------|
| 置顶/取消置顶当前窗口 | `Win` + `Ctrl` + `T` |
| 增加透明度 | `Win` + `Ctrl` + `=` |
| 降低透明度 | `Win` + `Ctrl` + `-` |
| 显示/隐藏 PinIt | `Win` + `Ctrl` + `P` |

## 对比其他工具

PowerToys 适合需要二十多种工具的用户。而 PinIt 只专注于一件事，并且做到极致：

| 功能 | **PinIt** | PowerToys <sub>Always On Top 模块</sub> | DeskPins <sub>经典免费软件</sub> |
|------|:---------:|:---------------------------------------:|:-------------------------------:|
| 价格 | ✅ 免费 | ✅ 免费 | ✅ 免费 |
| 单一功能、轻量 | ✅ **是** | ❌ 完整工具套件 | ✅ 是 |
| 全局快捷键置顶 | ✅ `Win+Ctrl+T` | ✅ 是 | ✅ 是 |
| 真正的窗口透明度 | ✅ **20% 到 100%** | ❌ 不支持 ¹ | ❌ 不支持 |
| 重启后保留置顶 | ✅ **自动** | ❌ 重启后丢失 | ❌ 不支持 |
| 持续维护 | ✅ 是 | ✅ 是 | ❌ 最后更新 2018 年 |

> **简而言之：** 如果你只需要窗口置顶功能，PinIt 是最小巧的全能工具，也是唯一一个重启后能记住置顶状态的工具。

¹ PowerToys 的"不透明度"设置改变的是置顶窗口周围的高亮*边框*，而非窗口内容本身。真正的窗口透明度是一个长期存在的功能请求（[#26049](https://github.com/microsoft/PowerToys/issues/26049)）。

## 常见问题

### 如何在 Windows 11 中让窗口始终置顶？

Windows 没有内置的置顶按钮。安装 PinIt，点击你想要保持可见的窗口，然后按 `Win+Ctrl+T` — 该窗口就会一直保持在其他窗口之上，直到你取消置顶。

### 置顶窗口的快捷键是什么？

PinIt 的默认快捷键是 `Win+Ctrl+T` 切换置顶状态。

### 能否让窗口变透明/半透明？

可以 — 使用 PinIt 置顶窗口后，按 `Win+Ctrl+-` 降低透明度（最低到 20%），或按 `Win+Ctrl+=` 恢复完全不透明。每个置顶窗口保持独立的透明度。

### 重启后置顶窗口会保持置顶吗？

会。PinIt 将你的置顶信息（按应用保存，包含透明度）保存到 `%LOCALAPPDATA%\PinIt`，下次启动时自动重新置顶 — 这是 PowerToys 和 DeskPins 都做不到的。

### 对以管理员权限运行的应用有效吗？

Windows 安全机制（UIPI）会阻止普通应用修改以管理员权限运行的窗口。要置顶管理员窗口，请以管理员身份运行 PinIt。

### PinIt 是否免费开源？

是的 — PinIt 在 [Apache 2.0 许可证](LICENSE) 下完全免费开源。你可以使用、修改和再分发，包括商业用途。

## 从源码构建

### 前置要求

- [Qt 6](https://www.qt.io/download-open-source)（Widgets）和 C++17 编译器（MinGW 或 MSVC）
- [CMake](https://cmake.org/) 3.21+

### 构建步骤

```bash
# 克隆仓库
git clone https://github.com/Razee4315/Pin-It.git
cd Pin-It

# 配置和构建（将 CMAKE_PREFIX_PATH 指向你的 Qt 安装路径）
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<Qt路径>
cmake --build build
```

可执行文件位于 `build/PinIt.exe`。要独立运行，使用 `windeployqt` 打包 Qt 运行时。要生成安装包，运行 `installer/PinIt.iss` 中的 [Inno Setup](https://jrsoftware.org/isinfo.php) 脚本。

> 每次推送都会通过 GitHub Actions（`.github/workflows/build.yml`）自动构建，生成安装包和便携版 ZIP。推送 `v*` 标签会自动发布到 GitHub Release。

## 为什么选择 PinIt？

作为一个经常在 Linux 和 Windows 之间切换的开发者，我一直怀念 Linux 桌面环境原生支持的窗口置顶功能。虽然 Linux 桌面环境通常内置了这个功能，但 Windows 上的选择却很有限。

最常见的解决方案 Microsoft PowerToys 捆绑了大量我不需要的工具。我想要一个轻量、单一功能的工具，只做一件事并且做到最好 — 没有臃肿。PinIt 诞生于对一个简洁、高效、像系统原生组件一样运行的替代方案的追求。

## 技术栈

- **语言**：C++17
- **UI**：Qt 6（Widgets），直接调用 Windows API
- **构建**：CMake + Inno Setup

> 最初使用 Rust + Tauri（v1.x）构建。之前的实现保留在 [`legacy-tauri`](https://github.com/Razee4315/Pin-It/tree/legacy-tauri) 分支。

## 许可证

PinIt 在 **[Apache License 2.0](LICENSE)** 下开源 — 免费使用、修改和再分发，包括商业用途。

## 作者

**Saqlain Abbas**
邮箱：saqlainrazee@gmail.com

GitHub：[@Razee4315](https://github.com/Razee4315)
