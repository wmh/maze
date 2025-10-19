# 🎯 迷宮產生器 Maze Generator

一個功能完整的迷宮產生專案，包含網頁版和 C 語言命令列版本，支援種子碼重現和列印輸出。

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Language](https://img.shields.io/badge/language-C%20%7C%20JavaScript-orange.svg)](https://github.com/wmh/maze)
[![Live Demo](https://img.shields.io/badge/demo-live-success.svg)](https://wmh.github.io/maze/)

## 🌐 線上體驗

**立即試玩：** [https://wmh.github.io/maze/](https://wmh.github.io/maze/)

無需安裝，直接在瀏覽器中體驗迷宮產生器！

## ✨ 功能特點

### 🌐 網頁版 (JavaScript)
- 🎨 精美的漸層介面設計
- 📏 可自訂迷宮尺寸 (5-50)
- 🎲 隨機生成迷宮或使用種子碼重現
- 🔢 種子碼系統（類似 Minecraft）
- 🟢 起點與 🔴 終點標記
- 🖨️ 列印功能
- 📱 響應式設計，支援各種螢幕尺寸
- ⚡ 即時互動生成

### 💻 C 語言版 (命令列)
- 📄 輸出 SVG 向量圖格式
- 🖨️ 適合列印到 A4 紙張 (210×297mm)
- 🎯 自動置中與尺寸計算
- 🌐 生成精美的 HTML 預覽頁面
- 🔢 支援種子碼重現相同迷宮
- 🎲 使用改良的迭代式 DFS 演算法
- 💾 純 C 語言實作，無外部依賴

## 🚀 快速開始

### 網頁版

**方法一：線上使用（推薦）**

直接訪問 GitHub Pages：[https://wmh.github.io/maze/](https://wmh.github.io/maze/)

**方法二：本地使用**

1. 直接用瀏覽器開啟 `index.html`
2. 輸入想要的寬度和高度
3. （選填）輸入種子碼以重現特定迷宮
4. 點擊「產生迷宮」按鈕
5. 點擊「🖨️ 列印」按鈕可直接列印

```bash
# 使用命令列開啟
xdg-open index.html  # Linux
open index.html      # macOS
start index.html     # Windows
```

### C 語言版

#### 編譯

```bash
# 使用 Makefile
make

# 或直接編譯
gcc -Wall -O2 -o maze maze.c
```

#### 使用方式

```bash
# 預設尺寸 (31x41，自動調整為奇數)
./maze

# 自訂尺寸
./maze 51 71

# 使用種子碼重現迷宮
./maze 31 41 12345

# 指定輸出檔名
./maze 41 51 12345 my_maze.svg
```

**命令格式：**
```
./maze [width] [height] [seed] [filename]
```

**注意**：寬度和高度會自動調整為奇數（演算法要求）。例如輸入 30×40 會自動調整為 31×41。

輸出檔案：
- `maze.svg` - SVG 向量圖檔
- `maze.html` - 可預覽和列印的 HTML 網頁（包含種子資訊）

#### 列印

開啟生成的 `maze.html`，點擊「🖨️ 列印迷宮」按鈕，或按 `Ctrl+P` (Windows/Linux) / `Cmd+P` (macOS)。

## 📁 專案結構

```
.
├── index.html          # 網頁版主檔案
├── style.css           # 網頁樣式（含列印樣式）
├── maze.js             # JavaScript 迷宮生成邏輯
├── maze.c              # C 語言版本源碼
├── Makefile            # 編譯配置
├── maze                # 編譯後的執行檔 (需編譯)
├── maze.svg            # 生成的 SVG 檔案 (執行後產生)
├── maze.html           # 生成的 HTML 檔案 (執行後產生)
├── docs/               # 文檔目錄
│   ├── TECHNICAL.md    # 技術文檔
│   └── (其他文檔)
├── .ai-context.md      # AI 協作上下文
└── README.md           # 本說明文件
```

## 🎨 迷宮生成演算法

使用**改良的迭代式 DFS + 隨機回溯**演算法生成迷宮：

1. 從起始點開始，標記為路徑
2. 使用棧記錄訪問路徑
3. **關鍵改進**：30% 機率隨機跳回較早的棧位置（創造分岔）
4. 70% 繼續正常 DFS 探索（保持連貫性）
5. 隨機選擇未訪問的相鄰格子
6. 移除兩格之間的牆
7. 重複直到所有格子都被訪問

此演算法保證生成的迷宮：
- ✅ 有唯一解（單一路徑從起點到終點）
- ✅ 完美迷宮 (Perfect Maze)
- ✅ 更多有意義的分岔路徑（比傳統遞迴回溯更複雜）
- ✅ 避免長走廊問題
- ✅ 使用種子碼可完美重現

## 🔢 種子碼系統

### 什麼是種子碼？

類似 Minecraft 的世界種子，使用相同的種子碼和尺寸，無論何時何地都能生成完全相同的迷宮。

### 如何使用？

**網頁版**：
1. 在「種子碼」欄位輸入數字（例如：12345）
2. 點擊「產生迷宮」
3. 分享種子碼給朋友，他們可以生成相同的迷宮！

**C 語言版**：
```bash
./maze 31 31 12345  # 每次執行都生成相同的迷宮
```

### 技術細節

使用 **Linear Congruential Generator (LCG)** 確保跨平台一致性：
- C 語言和 JavaScript 使用相同的隨機數公式
- 相同種子 + 相同尺寸 = 完全相同的迷宮
- 不依賴系統的 `rand()` 或 `Math.random()`

## 🛠️ 技術細節

### 網頁版技術棧
- **HTML5** - 結構
- **CSS3** - 樣式與動畫（含列印專用樣式）
- **Vanilla JavaScript** - 邏輯實作（無框架）

### C 語言版技術細節
- **標準 C** (C99)
- **無外部依賴** - 只使用標準函式庫
- **SVG 輸出** - 文字格式，易於調試
- **A4 尺寸適配** - 精確的 mm 單位控制

### 編譯器支援
- GCC 4.8+
- Clang 3.4+
- MSVC 2015+

## 📋 系統需求

### 網頁版
- 任何現代瀏覽器 (Chrome, Firefox, Safari, Edge)

### C 語言版
- C 編譯器 (GCC / Clang / MSVC)
- Make (選用，用於 Makefile)

## 🎯 使用範例

### 生成小型迷宮 (列印清晰)
```bash
./maze 21 31
```

### 生成中型迷宮 (適中難度)
```bash
./maze 41 51
```

### 使用種子碼挑戰朋友
```bash
./maze 31 31 88888
# 分享：「試試種子 88888 的 31×31 迷宮！」
```

### 重現特定迷宮
```bash
./maze 31 41 12345  # 使用種子 12345
```

## 🐛 清理編譯檔案

```bash
make clean
```

## 📜 授權

MIT License

## 👨‍💻 開發資訊

### 文檔
- [技術文檔](docs/TECHNICAL.md) - 詳細的技術說明和架構設計
- [AI 協作上下文](.ai-context.md) - AI 助手快速參考

### 版本歷史

**v2.0** - 2025-10-19
- ✨ 新增種子碼系統（可重現迷宮）
- 🎨 改良演算法（迭代式 DFS + 隨機回溯）
- 🖨️ 網頁版新增列印功能
- 📱 優化列印樣式
- 📝 完善文檔

**v1.0** - 2025-10-15
- 🎉 初始版本
- 基本迷宮生成功能（遞迴回溯）
- 網頁版和 C 語言版

## 🤝 貢獻

歡迎提交 Issue 和 Pull Request！

## 📧 聯絡

有問題或建議？歡迎開 Issue 討論！

---

⭐ 如果這個專案對你有幫助，歡迎給個 Star！
