#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_SIZE 100

// 簡單的 LCG 隨機數生成器（用於種子碼）
typedef struct {
    unsigned int seed;
} RandomGenerator;

RandomGenerator global_rng;

void init_random(unsigned int seed) {
    global_rng.seed = seed;
}

int next_random() {
    global_rng.seed = (1103515245 * global_rng.seed + 12345) & 0x7fffffff;
    return global_rng.seed;
}

int random_range(int max) {
    return next_random() % max;
}

typedef struct {
    int width;
    int height;
    int** grid;
    unsigned int seed;  // 種子碼
} Maze;

// 初始化迷宮
Maze* create_maze(int width, int height, unsigned int seed) {
    Maze* maze = (Maze*)malloc(sizeof(Maze));
    maze->width = width;
    maze->height = height;
    maze->seed = seed;
    
    // 分配二維陣列
    maze->grid = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        maze->grid[i] = (int*)malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) {
            maze->grid[i][j] = 1; // 1 = 牆
        }
    }
    
    // 初始化隨機數生成器
    init_random(seed);
    
    return maze;
}

// 釋放迷宮記憶體
void free_maze(Maze* maze) {
    for (int i = 0; i < maze->height; i++) {
        free(maze->grid[i]);
    }
    free(maze->grid);
    free(maze);
}

// 打亂陣列（Fisher-Yates shuffle，使用自訂隨機數）
void shuffle(int arr[][2], int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = random_range(i + 1);
        int temp0 = arr[i][0], temp1 = arr[i][1];
        arr[i][0] = arr[j][0];
        arr[i][1] = arr[j][1];
        arr[j][0] = temp0;
        arr[j][1] = temp1;
    }
}

// 使用改良的演算法生成更複雜的迷宮（迭代式 + 隨機回溯）
void generate_maze(Maze* maze) {
    // 使用棧實現迭代式 DFS，並隨機選擇回溯點
    typedef struct {
        int x, y;
    } Point;
    
    Point* stack = (Point*)malloc(maze->width * maze->height * sizeof(Point));
    int stack_size = 0;
    
    // 標記訪問過的格子
    int** visited = (int**)malloc(maze->height * sizeof(int*));
    for (int i = 0; i < maze->height; i++) {
        visited[i] = (int*)calloc(maze->width, sizeof(int));
    }
    
    // 從起點開始
    int x = 1, y = 1;
    maze->grid[y][x] = 0;
    visited[y][x] = 1;
    stack[stack_size++] = (Point){x, y};
    
    int directions[4][2] = {{0, -2}, {2, 0}, {0, 2}, {-2, 0}};
    
    while (stack_size > 0) {
        // 隨機選擇回溯點（而非總是最新的），增加分岔複雜度
        int stack_pos;
        if (stack_size > 10 && random_range(100) < 30) {
            // 30% 機率跳回較早的位置，創造更多分岔
            stack_pos = random_range(stack_size);
        } else {
            // 70% 使用最新位置（標準 DFS）
            stack_pos = stack_size - 1;
        }
        
        Point current = stack[stack_pos];
        x = current.x;
        y = current.y;
        
        // 找到所有未訪問的相鄰格子
        int unvisited[4][2];
        int unvisited_count = 0;
        
        for (int i = 0; i < 4; i++) {
            int dx = directions[i][0];
            int dy = directions[i][1];
            int newX = x + dx;
            int newY = y + dy;
            
            if (newX > 0 && newX < maze->width - 1 && 
                newY > 0 && newY < maze->height - 1 && 
                !visited[newY][newX]) {
                unvisited[unvisited_count][0] = dx;
                unvisited[unvisited_count][1] = dy;
                unvisited_count++;
            }
        }
        
        if (unvisited_count > 0) {
            // 隨機選擇一個方向
            int choice = random_range(unvisited_count);
            int dx = unvisited[choice][0];
            int dy = unvisited[choice][1];
            int newX = x + dx;
            int newY = y + dy;
            
            // 打通牆
            maze->grid[y + dy/2][x + dx/2] = 0;
            maze->grid[newY][newX] = 0;
            visited[newY][newX] = 1;
            
            // 加入新格子到棧
            stack[stack_size++] = (Point){newX, newY};
        } else {
            // 沒有未訪問的鄰居，從棧中移除
            stack[stack_pos] = stack[stack_size - 1];
            stack_size--;
        }
    }
    
    // 清理
    free(stack);
    for (int i = 0; i < maze->height; i++) {
        free(visited[i]);
    }
    free(visited);
    
    // 設置起點和終點
    maze->grid[1][1] = 2; // 起點
    maze->grid[maze->height - 2][maze->width - 2] = 3; // 終點
}

// 輸出 SVG 檔案
void export_svg(Maze* maze, const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        printf("無法建立檔案 %s\n", filename);
        return;
    }
    
    // A4 尺寸：210mm x 297mm，留邊距 10mm
    double page_width = 210;
    double page_height = 297;
    double margin = 10;
    double drawable_width = page_width - 2 * margin;
    double drawable_height = page_height - 2 * margin;
    
    // 計算每個格子的大小
    double cell_width = drawable_width / maze->width;
    double cell_height = drawable_height / maze->height;
    double cell_size = (cell_width < cell_height) ? cell_width : cell_height;
    
    // 置中
    double offset_x = margin + (drawable_width - cell_size * maze->width) / 2;
    double offset_y = margin + (drawable_height - cell_size * maze->height) / 2;
    
    // 寫入 SVG 標頭
    fprintf(fp, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"%gmm\" height=\"%gmm\" viewBox=\"0 0 %g %g\">\n",
            page_width, page_height, page_width, page_height);
    fprintf(fp, "  <rect width=\"%g\" height=\"%g\" fill=\"white\"/>\n", page_width, page_height);
    fprintf(fp, "  <g id=\"maze\">\n");
    
    // 繪製迷宮
    for (int y = 0; y < maze->height; y++) {
        for (int x = 0; x < maze->width; x++) {
            double px = offset_x + x * cell_size;
            double py = offset_y + y * cell_size;
            
            if (maze->grid[y][x] == 1) {
                // 牆 - 黑色
                fprintf(fp, "    <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"black\"/>\n",
                        px, py, cell_size, cell_size);
            } else if (maze->grid[y][x] == 2) {
                // 起點 - 綠色
                fprintf(fp, "    <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"#4CAF50\"/>\n",
                        px, py, cell_size, cell_size);
                fprintf(fp, "    <text x=\"%g\" y=\"%g\" font-size=\"%g\" text-anchor=\"middle\" dominant-baseline=\"middle\" fill=\"white\" font-weight=\"bold\">起</text>\n",
                        px + cell_size/2, py + cell_size/2, cell_size * 0.6);
            } else if (maze->grid[y][x] == 3) {
                // 終點 - 紅色
                fprintf(fp, "    <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"#f44336\"/>\n",
                        px, py, cell_size, cell_size);
                fprintf(fp, "    <text x=\"%g\" y=\"%g\" font-size=\"%g\" text-anchor=\"middle\" dominant-baseline=\"middle\" fill=\"white\" font-weight=\"bold\">終</text>\n",
                        px + cell_size/2, py + cell_size/2, cell_size * 0.6);
            }
        }
    }
    
    // 加上外框
    fprintf(fp, "    <rect x=\"%g\" y=\"%g\" width=\"%g\" height=\"%g\" fill=\"none\" stroke=\"black\" stroke-width=\"0.5\"/>\n",
            offset_x, offset_y, maze->width * cell_size, maze->height * cell_size);
    
    fprintf(fp, "  </g>\n");
    fprintf(fp, "</svg>\n");
    
    fclose(fp);
    printf("迷宮已儲存到 %s\n", filename);
}

// 輸出 HTML 檔案（包含美觀的網頁版面）
void export_html(Maze* maze, const char* svg_filename, const char* html_filename) {
    FILE* fp = fopen(html_filename, "w");
    if (!fp) {
        printf("無法建立檔案 %s\n", html_filename);
        return;
    }
    
    // 取得當前時間
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char datetime[100];
    strftime(datetime, sizeof(datetime), "%Y年%m月%d日 %H:%M:%S", t);
    
    fprintf(fp, "<!DOCTYPE html>\n");
    fprintf(fp, "<html lang=\"zh-TW\">\n");
    fprintf(fp, "<head>\n");
    fprintf(fp, "    <meta charset=\"UTF-8\">\n");
    fprintf(fp, "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
    fprintf(fp, "    <title>迷宮 - %dx%d</title>\n", maze->width, maze->height);
    fprintf(fp, "    <style>\n");
    fprintf(fp, "        * {\n");
    fprintf(fp, "            margin: 0;\n");
    fprintf(fp, "            padding: 0;\n");
    fprintf(fp, "            box-sizing: border-box;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        body {\n");
    fprintf(fp, "            font-family: 'Microsoft JhengHei', 'PingFang TC', 'Heiti TC', sans-serif;\n");
    fprintf(fp, "            background: linear-gradient(135deg, #667eea 0%%, #764ba2 100%%);\n");
    fprintf(fp, "            min-height: 100vh;\n");
    fprintf(fp, "            display: flex;\n");
    fprintf(fp, "            flex-direction: column;\n");
    fprintf(fp, "            align-items: center;\n");
    fprintf(fp, "            padding: 40px 20px;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .container {\n");
    fprintf(fp, "            background: white;\n");
    fprintf(fp, "            border-radius: 20px;\n");
    fprintf(fp, "            padding: 40px;\n");
    fprintf(fp, "            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);\n");
    fprintf(fp, "            max-width: 95vw;\n");
    fprintf(fp, "            animation: fadeIn 0.5s ease-out;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        @keyframes fadeIn {\n");
    fprintf(fp, "            from { opacity: 0; transform: translateY(-20px); }\n");
    fprintf(fp, "            to { opacity: 1; transform: translateY(0); }\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        h1 {\n");
    fprintf(fp, "            text-align: center;\n");
    fprintf(fp, "            color: #333;\n");
    fprintf(fp, "            margin-bottom: 10px;\n");
    fprintf(fp, "            font-size: 2.5em;\n");
    fprintf(fp, "            background: linear-gradient(135deg, #667eea 0%%, #764ba2 100%%);\n");
    fprintf(fp, "            -webkit-background-clip: text;\n");
    fprintf(fp, "            -webkit-text-fill-color: transparent;\n");
    fprintf(fp, "            background-clip: text;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .info {\n");
    fprintf(fp, "            text-align: center;\n");
    fprintf(fp, "            color: #666;\n");
    fprintf(fp, "            margin-bottom: 30px;\n");
    fprintf(fp, "            font-size: 0.95em;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .info span {\n");
    fprintf(fp, "            display: inline-block;\n");
    fprintf(fp, "            margin: 0 10px;\n");
    fprintf(fp, "            padding: 8px 16px;\n");
    fprintf(fp, "            background: #f5f5f5;\n");
    fprintf(fp, "            border-radius: 20px;\n");
    fprintf(fp, "            font-weight: bold;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .legend {\n");
    fprintf(fp, "            display: flex;\n");
    fprintf(fp, "            justify-content: center;\n");
    fprintf(fp, "            gap: 30px;\n");
    fprintf(fp, "            margin-bottom: 30px;\n");
    fprintf(fp, "            flex-wrap: wrap;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .legend-item {\n");
    fprintf(fp, "            display: flex;\n");
    fprintf(fp, "            align-items: center;\n");
    fprintf(fp, "            gap: 10px;\n");
    fprintf(fp, "            font-size: 0.95em;\n");
    fprintf(fp, "            color: #555;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .legend-box {\n");
    fprintf(fp, "            width: 30px;\n");
    fprintf(fp, "            height: 30px;\n");
    fprintf(fp, "            border-radius: 5px;\n");
    fprintf(fp, "            box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .maze-wrapper {\n");
    fprintf(fp, "            display: flex;\n");
    fprintf(fp, "            justify-content: center;\n");
    fprintf(fp, "            margin-bottom: 30px;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .maze-svg {\n");
    fprintf(fp, "            border: 3px solid #333;\n");
    fprintf(fp, "            border-radius: 10px;\n");
    fprintf(fp, "            box-shadow: 0 10px 30px rgba(0, 0, 0, 0.2);\n");
    fprintf(fp, "            max-width: 100%%;\n");
    fprintf(fp, "            height: auto;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .actions {\n");
    fprintf(fp, "            display: flex;\n");
    fprintf(fp, "            justify-content: center;\n");
    fprintf(fp, "            gap: 15px;\n");
    fprintf(fp, "            flex-wrap: wrap;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .btn {\n");
    fprintf(fp, "            padding: 12px 30px;\n");
    fprintf(fp, "            background: linear-gradient(135deg, #667eea 0%%, #764ba2 100%%);\n");
    fprintf(fp, "            color: white;\n");
    fprintf(fp, "            border: none;\n");
    fprintf(fp, "            border-radius: 25px;\n");
    fprintf(fp, "            font-size: 16px;\n");
    fprintf(fp, "            font-weight: bold;\n");
    fprintf(fp, "            cursor: pointer;\n");
    fprintf(fp, "            transition: transform 0.2s, box-shadow 0.2s;\n");
    fprintf(fp, "            text-decoration: none;\n");
    fprintf(fp, "            display: inline-block;\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .btn:hover {\n");
    fprintf(fp, "            transform: translateY(-2px);\n");
    fprintf(fp, "            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        .btn:active {\n");
    fprintf(fp, "            transform: translateY(0);\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "        @media print {\n");
    fprintf(fp, "            body {\n");
    fprintf(fp, "                background: white;\n");
    fprintf(fp, "                padding: 0;\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "            .container {\n");
    fprintf(fp, "                box-shadow: none;\n");
    fprintf(fp, "                padding: 0;\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "            h1, .info, .legend, .actions {\n");
    fprintf(fp, "                display: none;\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "            .maze-svg {\n");
    fprintf(fp, "                border: none;\n");
    fprintf(fp, "                box-shadow: none;\n");
    fprintf(fp, "            }\n");
    fprintf(fp, "        }\n");
    fprintf(fp, "    </style>\n");
    fprintf(fp, "</head>\n");
    fprintf(fp, "<body>\n");
    fprintf(fp, "    <div class=\"container\">\n");
    fprintf(fp, "        <h1>🎯 迷宮挑戰</h1>\n");
    fprintf(fp, "        <div class=\"info\">\n");
    fprintf(fp, "            <span>尺寸: %d × %d</span>\n", maze->width, maze->height);
    fprintf(fp, "            <span>種子碼: %u</span>\n", maze->seed);
    fprintf(fp, "            <span>生成時間: %s</span>\n", datetime);
    fprintf(fp, "        </div>\n");
    fprintf(fp, "        <div class=\"legend\">\n");
    fprintf(fp, "            <div class=\"legend-item\">\n");
    fprintf(fp, "                <div class=\"legend-box\" style=\"background: #4CAF50;\"></div>\n");
    fprintf(fp, "                <span>起點</span>\n");
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"legend-item\">\n");
    fprintf(fp, "                <div class=\"legend-box\" style=\"background: #f44336;\"></div>\n");
    fprintf(fp, "                <span>終點</span>\n");
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"legend-item\">\n");
    fprintf(fp, "                <div class=\"legend-box\" style=\"background: white; border: 2px solid #ddd;\"></div>\n");
    fprintf(fp, "                <span>路徑</span>\n");
    fprintf(fp, "            </div>\n");
    fprintf(fp, "            <div class=\"legend-item\">\n");
    fprintf(fp, "                <div class=\"legend-box\" style=\"background: black;\"></div>\n");
    fprintf(fp, "                <span>牆壁</span>\n");
    fprintf(fp, "            </div>\n");
    fprintf(fp, "        </div>\n");
    fprintf(fp, "        <div class=\"maze-wrapper\">\n");
    fprintf(fp, "            <img src=\"%s\" alt=\"迷宮\" class=\"maze-svg\">\n", svg_filename);
    fprintf(fp, "        </div>\n");
    fprintf(fp, "        <div class=\"actions\">\n");
    fprintf(fp, "            <button class=\"btn\" onclick=\"window.print()\">🖨️ 列印迷宮</button>\n");
    fprintf(fp, "            <a href=\"%s\" download class=\"btn\">💾 下載 SVG</a>\n", svg_filename);
    fprintf(fp, "        </div>\n");
    fprintf(fp, "    </div>\n");
    fprintf(fp, "</body>\n");
    fprintf(fp, "</html>\n");
    
    fclose(fp);
    printf("HTML 檔案已儲存到 %s\n", html_filename);
}

int main(int argc, char* argv[]) {
    int width = 30;
    int height = 40;
    unsigned int seed = 0;  // 0 表示使用時間作為種子
    char svg_filename[256] = "maze.svg";
    char html_filename[256] = "maze.html";
    
    // 解析命令列參數: ./maze [width] [height] [seed] [filename]
    if (argc >= 2) {
        width = atoi(argv[1]);
    }
    if (argc >= 3) {
        height = atoi(argv[2]);
    }
    if (argc >= 4) {
        seed = (unsigned int)atoi(argv[3]);
    }
    if (argc >= 5) {
        strncpy(svg_filename, argv[4], sizeof(svg_filename) - 1);
        // 自動產生對應的 HTML 檔名
        snprintf(html_filename, sizeof(html_filename), "%s", svg_filename);
        char* dot = strrchr(html_filename, '.');
        if (dot) {
            strcpy(dot, ".html");
        } else {
            strcat(html_filename, ".html");
        }
    }
    
    // 確保尺寸為奇數
    if (width % 2 == 0) width++;
    if (height % 2 == 0) height++;
    
    // 限制範圍
    if (width < 5) width = 5;
    if (height < 5) height = 5;
    if (width > MAX_SIZE) width = MAX_SIZE;
    if (height > MAX_SIZE) height = MAX_SIZE;
    
    // 如果沒有指定種子，使用時間
    if (seed == 0) {
        seed = (unsigned int)time(NULL);
    }
    
    printf("產生迷宮：%d x %d (種子: %u)\n", width, height, seed);
    
    // 建立並生成迷宮
    Maze* maze = create_maze(width, height, seed);
    generate_maze(maze);
    
    // 輸出 SVG
    export_svg(maze, svg_filename);
    
    // 輸出 HTML
    export_html(maze, svg_filename, html_filename);
    
    // 清理
    free_maze(maze);
    
    printf("完成！開啟 %s 即可預覽和列印\n", html_filename);
    printf("使用種子 %u 可重現此迷宮\n", seed);
    
    return 0;
}
