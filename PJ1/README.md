# 祖玛命令行版

数据结构课程项目 —— 祖玛彩球连锁消除游戏的命令行实现。

## 环境依赖

| 组件 | 版本要求 | 说明 |
|---|---|---|
| 操作系统 | Windows 10/11 | 使用了 `windows.h`（控制台着色、计时、日期）与 `conio.h` |
| 编译器 | g++ (支持 C++11) | 推荐 MSYS2 UCRT64 的 g++，兼容 MinGW-w64 |
| 构建工具 | GNU Make | 用于执行 Makefile（也可手动编译） |

程序仅依赖标准库与 Windows 系统 API，无第三方库。

## 安装（MSYS2 环境）

1. 下载并安装 [MSYS2](https://www.msys2.org/)

2. 打开 "MSYS2 UCRT64" 终端，安装编译器与 make：

   ```
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc make
   ```

3. 将 `C:\msys64\ucrt64\bin` 加入系统 PATH（安装 MSYS2 时一般已自动添加）

4. 验证安装：

   ```
   g++ --version
   make --version
   ```

> 若使用 Dev-Cpp / MinGW-w64 自带的 g++，同样可将对应 `bin` 目录加入 PATH；
> 若只有 `mingw32-make` 而无 `make`，把 `mingw32-make.exe` 复制一份为 `make.exe` 即可，
> 或直接用 `mingw32-make` 代替下文中的 `make`。

## 编译

### 使用 Makefile

```
make          # 编译生成 zuma.exe
make clean    # 清理 .o 与 zuma.exe
make rebuild  # 重新编译
```

### 手动编译

```
g++ Init.cpp GameModule.cpp FileReader.cpp TerminalUI.cpp Tunnel.cpp RankBoard.cpp -o zuma.exe
```

## 运行

```
zuma.exe
```

> 注意：程序读取 `levels/` 下的关卡文件、写入 `saves/` 下的存档与排行榜，
> 请在项目根目录下运行。

## 游戏规则

- 每回合将当前彩球插入轨道指定位置，或直接丢弃
- 插入后若形成 3 个及以上同色连续球则消除，消除后两侧接上可能触发连锁消除
- 每次有效回合结束后，等待区（FEED）有一个球进入轨道
- 轨道球数超过上限则失败；普通 / 变色龙模式达到目标分数即胜利（轨道与等待区全部清空但未达标同样失败）
- 弹药无限循环，不会耗尽

## 游戏模式

| 模式 | 说明 |
|---|---|
| 普通 | 达到关卡**目标分数**（`SCORE`）即通关；轨道超限或清空全部球但未达标则失败 |
| 无尽 | FEED 用完后随机补球，直到轨道超限失败 |
| 生存 | 无尽模式 + 限时（默认 60 秒，可在设置中调整），超时或轨道超限即失败 |
| 变色龙 | 胜利条件同普通（目标分数），但每次释放彩球后轨道中随机一球变为随机异色（以 `^` 标出） |

## 新手教程

选关列表第一项为教程关卡（`levels/tutorial.txt`，`TUTORIAL=1`）。教程的轨道 / FEED / 弹药固定，共 4 步引导：

1. 按提示输入数字，把彩球插入箭头 `^` 所指位置，凑成 3 连消除
2. 观察回合结束后 FEED 的球进入轨道尾部
3. 按 `d` 丢弃无用的当前球
4. 达到目标分数获胜

教程每一步只接受提示中的操作，输入其他内容会给出提示且不消耗回合（`p` / `q` / `r` 仍可用）；教程结算不计入排行榜。

## 操作说明

### 主菜单

```
1. Start Game   → 选关卡 → 选模式 → 开始游戏
2. Load Save    → 读取上次存档继续
3. Rankboard    → 查看排行榜
M. Settings     → 设置（生存限时 / 音效开关）
0. Quit         → 退出
```

### 选关卡 / 选模式

```
1. Tutorial（新手引导，直接进入，无需选模式）
2. Level 1
...
8. Level 7
0. Back（返回上一级）
```

```
1. Normal
2. Endless
3. Survival
4. Chameleon
0. Back（返回上一级）
```

### 游玩中

| 输入 | 功能 |
|---|---|
| 数字（如 `3`） | 将当前球插入到该位置（默认操作） |
| `i` | 插入（随后按提示输入位置） |
| `d` | 丢弃当前球 |
| `u` | 撤销上一步（最多 100 步） |
| `r` | 重新开始当前关卡 |
| `s` | 快捷存档（不中断游戏） |
| `p` | 暂停（继续 / 回主菜单） |
| `q` | 回主菜单 |

> 输入数字即默认为插入；只有输入字母才会执行对应功能。

### 暂停菜单

```
1. Continue        继续游戏
2. Quit to Main Menu  回主菜单
```

### 设置菜单

```
1. Survival Time Limit  修改生存模式起始限时（10 - 600 秒，默认 60 秒）
2. Sound Effect         音效开关
0. Back
```

## 存档与排行榜

- 游玩中按 `s` 或暂停前存档，写入 `saves/save.txt`
- 主菜单选 `2. Load Save` 读取存档
- 游戏结束后输入名字，记录写入 `saves/rank.txt`
- 排行榜：通关按用时升序，未通关按分数降序，各显示前 10 名

## 关卡文件

放在 `levels/` 目录下，格式：

```
ACTIVE=RRBBRR
FEED=GBYPRGBGYRBPYG
AMMO=BBGYRRGBYPRG
MAX_LENGTH=12
TUTORIAL=0
SCORE=240
```

- `ACTIVE`：初始轨道上的球
- `FEED`：等待进入轨道的球（每回合进一个）
- `AMMO`：弹药循环序列（第一个字符为初始当前球，第二个为初始下一个球）
- `MAX_LENGTH`：轨道球数上限，超过则失败
- `TUTORIAL`：可选，置 `1` 时该关为引导教程（启用固定操作脚本与提示）
- `SCORE`：目标分数，达到即通关（普通 / 变色龙模式生效）

内置关卡：

| 文件 | 说明 |
|---|---|
| `levels/tutorial.txt` | 新手引导（固定脚本，4 步教学） |
| `levels/level1.txt` ~ `level7.txt` | 普通关卡，球数与上限逐关提升 |

## 计分规则

```
得分 = 本次消除球数 × 20 × 加分系数
加分系数 = 1.0 + (本次消除球数 - 3) × 0.2
```

即消除球数越多，单个球的分值越高（3 个 = 60 分，4 个 = 96 分，5 个 = 140 分…）。

## 项目结构

| 文件 | 职责 |
|---|---|
| `GameModule.h/cpp` | 核心逻辑：球链、插入、消除、计分、变色标记、存档/读档、快照 |
| `FileReader.h/cpp` | 从 `levels/` 读取并解析关卡文件 |
| `TerminalUI.h/cpp` | 命令行显示、彩色输出、输入、限时输入、菜单、教学提示 |
| `Tunnel.h/cpp` | 游戏流程控制：回合循环、弹药、补球、计时、撤销、教程脚本、排行榜、设置 |
| `RankBoard.h/cpp` | 排行榜存储、排序、显示 |
| `Init.cpp` | 程序入口 |

## 使用的数据结构

| 结构 | 用途 |
|---|---|
| `std::string`（动态数组） | 球链轨道、弹药序列 |
| `std::deque<char>`（双端队列） | FEED 等待区，FIFO 补球 |
| `std::vector<char>`（并行标记数组） | 变色龙模式中被变色球的标记，与球链等长同步 |
| `std::vector<RankEntry>` | 排行榜记录列表 |
| `std::vector<GameModule::Snapshot>`（栈） | 撤销历史记录 |
| `struct LevelData` | 关卡配置数据聚合 |
| `struct RankEntry` | 单条排行记录聚合 |
| `std::function`（回调） | 消除过程的分步动画回调 |
| 取模索引（环形缓冲区思想） | 弹药循环生成 |
