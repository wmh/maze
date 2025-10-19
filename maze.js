// 簡單的隨機數生成器（用於種子碼）
class SeededRandom {
    constructor(seed) {
        this.seed = seed;
    }
    
    next() {
        this.seed = (this.seed * 1103515245 + 12345) & 0x7fffffff;
        return this.seed;
    }
    
    nextInt(max) {
        return this.next() % max;
    }
}

class MazeGenerator {
    constructor(width, height, seed = null) {
        this.width = width;
        this.height = height;
        this.seed = seed || Date.now();
        this.rng = new SeededRandom(this.seed);
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
        // 使用改良的演算法（迭代式 DFS + 隨機回溯）
        const stack = [];
        const visited = Array(this.height).fill(0).map(() => Array(this.width).fill(false));
        
        // 從起點開始
        let x = 1, y = 1;
        this.maze[y][x] = 0;
        visited[y][x] = true;
        stack.push({x, y});
        
        const directions = [[0, -2], [2, 0], [0, 2], [-2, 0]];
        
        while (stack.length > 0) {
            // 隨機選擇回溯點（增加分岔複雜度）
            let stackPos;
            if (stack.length > 10 && this.rng.nextInt(100) < 30) {
                // 30% 機率跳回較早的位置
                stackPos = this.rng.nextInt(stack.length);
            } else {
                // 70% 使用最新位置
                stackPos = stack.length - 1;
            }
            
            const current = stack[stackPos];
            x = current.x;
            y = current.y;
            
            // 找到所有未訪問的相鄰格子
            const unvisited = [];
            for (let [dx, dy] of directions) {
                const newX = x + dx;
                const newY = y + dy;
                
                if (this.isValid(newX, newY) && !visited[newY][newX]) {
                    unvisited.push({dx, dy});
                }
            }
            
            if (unvisited.length > 0) {
                // 隨機選擇一個方向
                const choice = unvisited[this.rng.nextInt(unvisited.length)];
                const dx = choice.dx;
                const dy = choice.dy;
                const newX = x + dx;
                const newY = y + dy;
                
                // 打通牆
                this.maze[y + dy / 2][x + dx / 2] = 0;
                this.maze[newY][newX] = 0;
                visited[newY][newX] = true;
                
                // 加入新格子到棧
                stack.push({x: newX, y: newY});
            } else {
                // 沒有未訪問的鄰居，從棧中移除
                stack[stackPos] = stack[stack.length - 1];
                stack.pop();
            }
        }
        
        // 設置起點和終點
        this.maze[1][1] = 2; // 2 = 起點
        this.maze[this.height - 2][this.width - 2] = 3; // 3 = 終點
    }

    isValid(x, y) {
        return x > 0 && x < this.width - 1 && y > 0 && y < this.height - 1;
    }

    getMaze() {
        return this.maze;
    }
    
    getSeed() {
        return this.seed;
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
    const seedInput = document.getElementById('seed').value.trim();
    
    // 確保尺寸為奇數（對於遞迴回溯算法）
    const adjustedWidth = width % 2 === 0 ? width + 1 : width;
    const adjustedHeight = height % 2 === 0 ? height + 1 : height;
    
    // 如果有輸入種子碼，使用該種子；否則使用時間戳
    const seed = seedInput ? parseInt(seedInput) : null;
    
    const generator = new MazeGenerator(adjustedWidth, adjustedHeight, seed);
    generator.generate();
    const maze = generator.getMaze();
    const usedSeed = generator.getSeed();
    
    // 顯示使用的種子碼
    document.getElementById('seedDisplay').textContent = `當前種子碼: ${usedSeed}`;
    document.getElementById('seed').value = usedSeed;
    
    renderMaze(maze);
}

// 事件監聽器
document.getElementById('generateBtn').addEventListener('click', generateMaze);
document.getElementById('printBtn').addEventListener('click', () => {
    window.print();
});

// 頁面載入時生成初始迷宮
window.addEventListener('load', generateMaze);
