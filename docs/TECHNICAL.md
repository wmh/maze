# 技術文檔 Technical Documentation

## 📋 專案概覽

迷宮產生器專案，包含網頁版（JavaScript）和命令列版（C 語言）兩個實作。

### 核心技術
- **演算法**: 改良的迭代式 DFS + 隨機回溯
- **種子系統**: 可重現的隨機生成（類似 Minecraft）
- **網頁版**: Vanilla JavaScript + HTML5 + 列印功能
- **命令列版**: C99 標準，輸出 SVG

---

## 🏗️ 架構設計

### 資料結構

**C 語言版 (maze.c)**
```c
typedef struct {
    int width;      // 迷宮寬度
    int height;     // 迷宮高度
    int** grid;     // 二維陣列：1=牆, 0=路徑
    unsigned int seed;  // 種子碼（用於重現）
} Maze;

typedef struct {
    unsigned int seed;
} RandomGenerator;  // LCG 隨機數生成器
```

**JavaScript 版 (maze.js)**
```javascript
class SeededRandom {
    seed  // 種子碼
    next()  // 下一個隨機數
    nextInt(max)  // 範圍內隨機整數
}

class MazeGenerator {
    this.width   // 迷宮寬度
    this.height  // 迷宮高度
    this.seed    // 種子碼
    this.maze    // 二維陣列：1=牆, 0=路徑, 2=起點, 3=終點
}
```

### 演算法說明

**改良的迭代式 DFS + 隨機回溯**

此版本解決傳統遞迴回溯的長走廊問題，產生更多有意義的分岔路徑。

1. **初始化**: 所有格子設為牆 (1)
2. **起點**: 從 (1,1) 開始標記為路徑 (0)
3. **棧管理**: 使用棧記錄訪問路徑
4. **隨機回溯** (關鍵改進):
   - 30% 機率：隨機跳回較早的棧位置（創造分岔）
   - 70% 機率：使用最新位置（正常 DFS）
5. **路徑探索**: 隨機選擇未訪問的相鄰格子
6. **打通牆壁**: 移除當前格子與目標格子之間的牆
7. **重複**: 直到棧為空（所有可達格子都被訪問）

**特性**:
- 保證單一路徑（無多重解）
- 產生更多短分岔（增加選擇困難度）
- 相同種子碼產生完全相同的迷宮
- 比傳統遞迴回溯有更多「決策點」
- 避免長走廊和裝飾性分岔

**與傳統遞迴回溯的差異**:
- 傳統：總是走到最深處才回溯 → 長走廊
- 改良：隨機跳回較早位置 → 自然分岔

### 種子系統

使用 **Linear Congruential Generator (LCG)** 確保跨平台一致性：

```
seed = (seed * 1103515245 + 12345) & 0x7fffffff
```

**優點**:
- 簡單高效
- 跨平台一致（C 和 JS 實作相同）
- 可重現任何迷宮
- 便於分享和挑戰

---

## 💻 程式碼慣例

### 命名規則

**C 語言**:
- 函式: `snake_case` (例: `create_maze`, `generate_maze`)
- 結構: `PascalCase` (例: `Maze`, `RandomGenerator`)
- 常數: `UPPER_CASE` (例: `MAX_SIZE`, `MAX_FRONTIER`)

**JavaScript**:
- 類別: `PascalCase` (例: `MazeGenerator`, `SeededRandom`)
- 方法/變數: `camelCase` (例: `generate`, `getSeed`)
- 常數: 無特別規範，使用 `const`

### 註解風格

- 簡潔為主，只在複雜邏輯處註解
- 中文註解方便維護
- 函式頂部簡短說明用途

---

## ⚙️ 參數與限制

### 網頁版

| 參數 | 範圍 | 預設值 | 說明 |
|------|------|--------|------|
| width | 5-50 | 20 | 迷宮寬度（必須為奇數） |
| height | 5-50 | 20 | 迷宮高度（必須為奇數） |
| seed | 任意整數 | Date.now() | 種子碼（留空自動生成） |

**尺寸限制原因**: 
- 小於 5: 太簡單無挑戰
- 大於 50: 影響渲染效能和視覺體驗

### C 語言版

| 參數 | 範圍 | 預設值 | 說明 |
|------|------|--------|------|
| width | 5-99 | 31 | 迷宮寬度（必須為奇數） |
| height | 5-99 | 41 | 迷宮高度（必須為奇數） |
| seed | 任意整數 | time(NULL) | 種子碼（0 = 自動） |

**命令格式**:
```bash
./maze [width] [height] [seed] [filename]
./maze 31 31 12345 custom.svg  # 使用種子 12345
./maze 41 41                    # 自動生成種子
```

**輸出規格**:
- 紙張: A4 (210×297mm)
- 邊距: 10mm
- 格子大小: 自動計算以適應頁面
- 檔案: SVG (向量圖) + HTML (預覽頁，包含種子資訊)

---

## 🎨 樣式與主題

### 網頁版顏色配置

```css
--gradient-start: #667eea    /* 漸層起點 - 紫色 */
--gradient-end: #764ba2      /* 漸層終點 - 深紫 */
--wall-color: #333           /* 牆壁 - 深灰 */
--path-color: #fff           /* 路徑 - 白色 */
--start-color: #4CAF50       /* 起點 - 綠色 */
--end-color: #f44336         /* 終點 - 紅色 */
```

### SVG 顏色配置 (maze.c)

```c
牆壁: fill="black" (黑色)
起點: fill="#4CAF50" (綠色) + 文字「起」
終點: fill="#f44336" (紅色) + 文字「終」
```

---

## 🔧 常見修改場景

### 1. 調整尺寸限制

**網頁版** (index.html):
```html
<input type="number" min="5" max="50" ...>  <!-- 修改 min/max -->
```

**C 語言版** (maze.c):
```c
#define MAX_SIZE 100  // 修改最大尺寸限制
#define MAX_FRONTIER 10000  // 修改 frontier 列表大小
```

### 2. 使用種子碼重現迷宮

**網頁版**: 在種子碼欄位輸入數字後點擊「產生迷宮」

**C 語言版**:
```bash
./maze 31 31 12345  # 每次執行都生成相同的迷宮
```

### 3. 更改顏色主題

**網頁版**: 編輯 `style.css` 的 CSS 變數  
**C 語言版**: 搜尋 `fill=` 修改顏色代碼

### 4. 調整演算法複雜度

目前使用 Prim's 演算法，特性是產生較多短分岔。如果想要更長的走廊，可以：

**修改選擇策略** (maze.c 和 maze.js):
- 當前: 隨機選擇 frontier (`random_range(rng, frontier_size)`)
- 改為 LIFO: 總是選最後加入的 (`frontier_size - 1`)
- 改為 FIFO: 總是選最先加入的 (`0`)

### 5. 調整列印尺寸

**maze.c** 的 `export_svg()` 函式:
```c
double page_width = 210;   // A4 寬度 (mm)
double page_height = 297;  // A4 高度 (mm)
double margin = 10;        // 邊距 (mm)
```

---

## 🚨 已知限制與注意事項

### 演算法特性

1. **單一解法**: Prim's 演算法保證只有一條從起點到終點的路徑
2. **短分岔較多**: 相比遞迴回溯，產生更多決策點但分岔較短
3. **種子依賴**: 相同種子+相同尺寸 = 相同迷宮（跨平台一致）

### 效能限制

1. **網頁版**: 
   - 超過 50×50 會有明顯卡頓
   - Frontier list 可能變大（記憶體使用增加）

2. **C 語言版**:
   - 記憶體使用: O(width × height)
   - Frontier list 限制: MAX_FRONTIER (10000 個格子)
   - 建議 < 100×100

### 輸入驗證

- **必須為奇數**: 演算法要求，偶數無法正確生成
- **最小值**: 至少 5×5 才能形成有意義的迷宮
- **種子碼**: 使用 32-bit 整數（0-2147483647）

### 跨平台一致性

- 使用自訂 LCG 隨機數生成器確保一致性
- C 和 JS 版本使用相同的種子會產生相同迷宮
- 不依賴系統的 `rand()` 或 `Math.random()`

---

## 🎯 待辦事項 / 未來改進

### 短期改進
- [ ] 加入難度選項（調整演算法參數）
- [ ] 網頁版加入解題功能（路徑尋找動畫）
- [ ] 支援更多演算法（Kruskal's, Eller's）
- [ ] 分享功能（URL 包含種子碼和尺寸）

### 長期改進
- [ ] 3D 迷宮生成
- [ ] 多起點/終點支援
- [ ] 匯出為 PNG/PDF
- [ ] 迷宮解題競賽模式（計時）

### 效能優化
- [ ] Web Worker 處理大型迷宮
- [ ] C 版使用動態 frontier（避免固定大小限制）

---

## 🛠️ 開發環境

### 編譯工具
- **GCC**: 4.8+ (推薦 9.0+)
- **Clang**: 3.4+
- **Make**: GNU Make 3.8+

### 測試環境
- **瀏覽器**: Chrome 90+, Firefox 88+, Safari 14+
- **作業系統**: Linux, macOS, Windows (WSL)

---

## 📝 與 AI 協作注意事項

### 修改前須知
1. **保持演算法一致性**: C 和 JS 版本應使用相同邏輯和種子系統
2. **奇偶數限制**: 寬高必須為奇數，這是演算法前提
3. **種子一致性**: 修改隨機數生成器會破壞種子重現功能
4. **測試雙版本**: 修改後確保兩版本都正常運作且種子一致

### 常見修改類型
- **UI 調整**: 只改 HTML/CSS，不動 maze.js 邏輯
- **演算法調整**: 同步修改 maze.c 和 maze.js
- **輸出格式**: 只改 `export_svg()` 和 `export_html()`
- **種子功能**: 謹慎修改，確保跨平台一致

### 程式碼風格
- **簡潔優先**: 保持程式碼可讀性
- **註解保留**: 現有中文註解請保留
- **向後相容**: 避免破壞現有功能
- **種子驗證**: 修改後測試相同種子是否產生相同迷宮

---

## 📚 相關資源

### 演算法參考
- [Prim's Maze Generation Algorithm](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Randomized_Prim's_algorithm)
- [Linear Congruential Generator](https://en.wikipedia.org/wiki/Linear_congruential_generator)
- [Perfect Maze](https://en.wikipedia.org/wiki/Maze_generation_algorithm)

### SVG 規範
- [SVG 1.1 Specification](https://www.w3.org/TR/SVG11/)

### 工具文檔
- [MDN Canvas API](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API)
- [GCC Manual](https://gcc.gnu.org/onlinedocs/)

---

**最後更新**: 2025-10-19  
**版本**: 2.0 (Prim's 演算法 + 種子系統)  
**維護者**: Generated by GitHub Copilot CLI

---

## 💻 程式碼慣例

### 命名規則

**C 語言**:
- 函式: `snake_case` (例: `create_maze`, `carve_passages`)
- 結構: `PascalCase` (例: `Maze`)
- 常數: `UPPER_CASE` (例: `MAX_SIZE`)

**JavaScript**:
- 類別: `PascalCase` (例: `MazeGenerator`)
- 方法/變數: `camelCase` (例: `carvePassages`, `cellSize`)
- 常數: 無特別規範，使用 `const`

### 註解風格

- 簡潔為主，只在複雜邏輯處註解
- 中文註解方便維護
- 函式頂部簡短說明用途

---

## ⚙️ 參數與限制

### 網頁版

| 參數 | 範圍 | 預設值 | 說明 |
|------|------|--------|------|
| width | 5-50 | 21 | 迷宮寬度（必須為奇數） |
| height | 5-50 | 21 | 迷宮高度（必須為奇數） |
| cellSize | auto | 計算 | 根據 canvas 大小自動調整 |

**尺寸限制原因**: 
- 小於 5: 太簡單無挑戰
- 大於 50: 影響渲染效能和視覺體驗

### C 語言版

| 參數 | 範圍 | 預設值 | 說明 |
|------|------|--------|------|
| width | 11-99 | 31 | 迷宮寬度（必須為奇數） |
| height | 11-99 | 41 | 迷宮高度（必須為奇數） |

**輸出規格**:
- 紙張: A4 (210×297mm)
- 邊距: 20mm
- 格子大小: 自動計算以適應頁面
- 檔案: SVG (向量圖) + HTML (預覽頁)

---

## 🎨 樣式與主題

### 網頁版顏色配置

```css
--gradient-start: #667eea    /* 漸層起點 - 紫色 */
--gradient-end: #764ba2      /* 漸層終點 - 深紫 */
--wall-color: #2d3748        /* 牆壁 - 深灰 */
--path-color: #ffffff        /* 路徑 - 白色 */
--start-color: #48bb78       /* 起點 - 綠色 */
--end-color: #f56565         /* 終點 - 紅色 */
```

### SVG 顏色配置 (maze.c)

```c
牆壁: stroke="#333" (深灰)
起點: fill="#4ade80" (綠色圓點)
終點: fill="#ef4444" (紅色圓點)
```

---

## 🔧 常見修改場景

### 1. 調整尺寸限制

**網頁版** (index.html):
```html
<input type="number" min="5" max="50" ...>  <!-- 修改 min/max -->
```

**C 語言版** (maze.c):
```c
#define MAX_SIZE 100  // 修改最大尺寸限制
```

### 2. 更改顏色主題

**網頁版**: 編輯 `style.css` 的 CSS 變數  
**C 語言版**: 搜尋 `stroke=` 和 `fill=` 修改顏色代碼

### 3. 調整列印尺寸

**maze.c** 的 `write_svg()` 函式:
```c
const double paper_width = 210;   // A4 寬度 (mm)
const double paper_height = 297;  // A4 高度 (mm)
const double margin = 20;         // 邊距 (mm)
```

### 4. 修改演算法行為

**起點/終點位置** (maze.c):
```c
maze->grid[1][1] = 2;  // 起點 - 左上角
maze->grid[height-2][width-2] = 3;  // 終點 - 右下角
```

**JavaScript 版**: 在 `maze.js` 的 `generate()` 方法中修改

---

## 🚨 已知限制與注意事項

### 效能限制

1. **網頁版**: 
   - 超過 50×50 會有明顯卡頓
   - Canvas 渲染在低階裝置可能較慢

2. **C 語言版**:
   - 記憶體使用: O(width × height)
   - 遞迴深度限制: 避免 stack overflow，建議 < 100×100

### 輸入驗證

- **必須為奇數**: 演算法要求，偶數無法正確生成
- **最小值**: 至少 5×5（或 11×11 for C）才能形成有意義的迷宮

### 瀏覽器相容性

- 需支援 ES6 (class, arrow function, const/let)
- Canvas API
- 現代瀏覽器皆支援（IE 不支援）

---

## 🎯 待辦事項 / 未來改進

### 短期改進
- [ ] 加入難度選項（影響路徑複雜度）
- [ ] 支援不同迷宮演算法（Prim's, Kruskal's）
- [ ] 網頁版加入解題功能（路徑尋找）

### 長期改進
- [ ] 3D 迷宮生成
- [ ] 多起點/終點支援
- [ ] 匯出為 PNG/PDF
- [ ] 迷宮解題動畫

### 效能優化
- [ ] Web Worker 處理大型迷宮
- [ ] C 版使用迭代代替遞迴（避免 stack overflow）

---

## 🛠️ 開發環境

### 編譯工具
- **GCC**: 4.8+ (推薦 9.0+)
- **Clang**: 3.4+
- **Make**: GNU Make 3.8+

### 測試環境
- **瀏覽器**: Chrome 90+, Firefox 88+, Safari 14+
- **作業系統**: Linux, macOS, Windows (WSL)

---

## 📝 與 AI 協作注意事項

### 修改前須知
1. **保持演算法一致性**: C 和 JS 版本應使用相同邏輯
2. **奇偶數限制**: 寬高必須為奇數，這是演算法前提
3. **測試雙版本**: 修改後確保兩版本都正常運作

### 常見修改類型
- **UI 調整**: 只改 HTML/CSS，不動 maze.js 邏輯
- **演算法調整**: 同步修改 maze.c 和 maze.js
- **輸出格式**: 只改 write_svg() 和 write_html()

### 程式碼風格
- **簡潔優先**: 保持程式碼可讀性
- **註解保留**: 現有中文註解請保留
- **向後相容**: 避免破壞現有功能

---

## 📚 相關資源

### 演算法參考
- [Recursive Backtracking Algorithm](https://en.wikipedia.org/wiki/Maze_generation_algorithm#Recursive_backtracker)
- [Perfect Maze](https://en.wikipedia.org/wiki/Maze_generation_algorithm)

### SVG 規範
- [SVG 1.1 Specification](https://www.w3.org/TR/SVG11/)

### 工具文檔
- [MDN Canvas API](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API)
- [GCC Manual](https://gcc.gnu.org/onlinedocs/)

---

**最後更新**: 2025-10-19  
**維護者**: Generated by GitHub Copilot CLI
