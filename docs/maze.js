class MazeGenerator {
    constructor(width, height) {
        this.width = width;
        this.height = height;
        this.maze = [];
        this.initialize();
    }

    initialize() {
        // 創建迷宮網格，全部初始化為牆
        for (let y = 0; y < this.height; y++) {
            this.maze[y] = [];
            for (let x = 0; x < this.width; x++) {
                this.maze[y][x] = 1; // 1 = 牆
            }
        }
    }

    generate() {
        // 使用遞迴回溯算法 (Recursive Backtracking)
        const startX = 1;
        const startY = 1;
        this.maze[startY][startX] = 0; // 0 = 路徑
        this.carvePassages(startX, startY);
        
        // 設置起點和終點
        this.maze[1][1] = 2; // 2 = 起點
        this.maze[this.height - 2][this.width - 2] = 3; // 3 = 終點
    }

    carvePassages(x, y) {
        // 四個方向：上、右、下、左
        const directions = [
            [0, -2], // 上
            [2, 0],  // 右
            [0, 2],  // 下
            [-2, 0]  // 左
        ];
        
        // 隨機打亂方向
        this.shuffle(directions);
        
        for (let [dx, dy] of directions) {
            const newX = x + dx;
            const newY = y + dy;
            
            // 檢查新位置是否在迷宮範圍內且是牆
            if (this.isValid(newX, newY) && this.maze[newY][newX] === 1) {
                // 打通當前格子到新格子之間的牆
                this.maze[y + dy / 2][x + dx / 2] = 0;
                this.maze[newY][newX] = 0;
                
                // 遞迴處理新格子
                this.carvePassages(newX, newY);
            }
        }
    }

    isValid(x, y) {
        return x > 0 && x < this.width - 1 && y > 0 && y < this.height - 1;
    }

    shuffle(array) {
        for (let i = array.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [array[i], array[j]] = [array[j], array[i]];
        }
    }

    getMaze() {
        return this.maze;
    }
}

function renderMaze(maze) {
    const mazeDiv = document.getElementById('maze');
    mazeDiv.innerHTML = '';
    
    const table = document.createElement('table');
    table.className = 'maze-table';
    
    for (let y = 0; y < maze.length; y++) {
        const row = document.createElement('tr');
        for (let x = 0; x < maze[y].length; x++) {
            const cell = document.createElement('td');
            cell.className = 'maze-cell';
            
            switch (maze[y][x]) {
                case 0:
                    cell.classList.add('path');
                    break;
                case 1:
                    cell.classList.add('wall');
                    break;
                case 2:
                    cell.classList.add('start');
                    break;
                case 3:
                    cell.classList.add('end');
                    break;
            }
            
            row.appendChild(cell);
        }
        table.appendChild(row);
    }
    
    mazeDiv.appendChild(table);
}

function generateMaze() {
    const width = parseInt(document.getElementById('width').value);
    const height = parseInt(document.getElementById('height').value);
    
    // 確保尺寸為奇數（對於遞迴回溯算法）
    const adjustedWidth = width % 2 === 0 ? width + 1 : width;
    const adjustedHeight = height % 2 === 0 ? height + 1 : height;
    
    const generator = new MazeGenerator(adjustedWidth, adjustedHeight);
    generator.generate();
    const maze = generator.getMaze();
    
    renderMaze(maze);
}

// 事件監聽器
document.getElementById('generateBtn').addEventListener('click', generateMaze);

// 頁面載入時生成初始迷宮
window.addEventListener('load', generateMaze);
