# 🎯 迷宮產生器 Maze Generator

一個功能完整的迷宮產生專案，包含網頁版和 C 語言命令列版本，支援隨機迷宮生成與列印輸出。

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Language](https://img.shields.io/badge/language-C%20%7C%20JavaScript-orange.svg)

## ✨ 功能特點

### 🌐 網頁版 (JavaScript)
- 🎨 精美的漸層介面設計
- 📏 可自訂迷宮尺寸 (5-50)
- 🎲 隨機生成迷宮
- 🟢 起點與 🔴 終點標記
- 📱 響應式設計，支援各種螢幕尺寸
- ⚡ 即時互動生成

### 💻 C 語言版 (命令列)
- 📄 輸出 SVG 向量圖格式
- 🖨️ 適合列印到 A4 紙張 (210×297mm)
- 🎯 自動置中與尺寸計算
- 🌐 生成精美的 HTML 預覽頁面
- 🎲 使用遞迴回溯演算法生成迷宮
- 💾 純 C 語言實作，無外部依賴

## 🚀 快速開始

### 網頁版

1. 直接用瀏覽器開啟 `index.html`
2. 輸入想要的寬度和高度
3. 點擊「產生迷宮」按鈕

```bash
# 或使用命令列開啟
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
# 預設尺寸 (31x41)
./maze

# 自訂尺寸
./maze 51 71

# 指定輸出檔名
./maze 41 51 my_maze.svg
```

輸出檔案：
- `maze.svg` - SVG 向量圖檔
- `maze.html` - 可預覽和列印的 HTML 網頁

#### 列印

開啟生成的 `maze.html`，點擊「🖨️ 列印迷宮」按鈕，或按 `Ctrl+P` (Windows/Linux) / `Cmd+P` (macOS)。

## 📁 專案結構

```
.
├── index.html          # 網頁版主檔案
├── style.css           # 網頁樣式
├── maze.js             # JavaScript 迷宮生成邏輯
├── maze.c              # C 語言版本源碼
├── Makefile            # 編譯配置
├── maze                # 編譯後的執行檔 (需編譯)
├── maze.svg            # 生成的 SVG 檔案 (執行後產生)
├── maze.html           # 生成的 HTML 檔案 (執行後產生)
├── log/                # 開發日誌
│   ├── 2025-10-15.txt  # 純文字格式
│   ├── 2025-10-15.html # HTML 格式
│   └── 2025-10-15.md   # Markdown 格式
└── README.md           # 本說明文件
```

## 🎨 迷宮生成演算法

使用**遞迴回溯演算法 (Recursive Backtracking)** 生成迷宮：

1. 從起始點開始，標記為路徑
2. 隨機選擇一個相鄰的未訪問格子
3. 移除兩格之間的牆
4. 遞迴處理新格子
5. 如果沒有未訪問的相鄰格子，回溯到上一個格子
6. 重複直到所有格子都被訪問

此演算法保證生成的迷宮：
- ✅ 有唯一解
- ✅ 從起點可到達終點
- ✅ 無迴圈
- ✅ 完美迷宮 (Perfect Maze)

## 🛠️ 技術細節

### 網頁版技術棧
- **HTML5** - 結構
- **CSS3** - 樣式與動畫
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

### 生成大型迷宮 (高難度)
```bash
./maze 61 81
```

## 🐛 清理編譯檔案

```bash
make clean
```

## 📜 授權

MIT License

## 👨‍💻 開發日誌

詳細的開發過程記錄在 [log/](./log/) 目錄中，包含：
- 專案構思與需求討論
- 技術選型與實作細節
- 問題解決過程

查看 [log/2025-10-15.md](./log/2025-10-15.md) 了解本專案的完整開發歷程。

## 🤝 貢獻

歡迎提交 Issue 和 Pull Request！

## 📧 聯絡

有問題或建議？歡迎開 Issue 討論！

---

⭐ 如果這個專案對你有幫助，歡迎給個 Star！
