#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <cstdlib>

#define H 20
#define W 15
#define VIEWPORT_HEIGHT 20 

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

const char BLOCK = char(219);
int times = 4, score = 0, nextBlock = -1;
char board[H][W] = {};
int boardcolor[H][W] = {};

using namespace std;

int getRandomColor() {
    return (rand() % 15) + 1;
}

// --- LỚP CHA BLOCK ---
class Block {
protected:
    int rotation;
    int color;
    int x, y; 

public:
    Block(int c) : rotation(0), color(c), x(4), y(0) {}
    virtual ~Block() {}

    void setPos(int nx, int ny) { x = nx; y = ny; }
    void setRotation(int r) { rotation = r % 4; }
    int getX() { return x; }
    int getY() { return y; }
    int getRotation() { return rotation; }
    int getColor() { return color; };

    void applyColor() const {
        SetConsoleTextAttribute(hConsole, color);
    }

    static void resetColor() {
        SetConsoleTextAttribute(hConsole, 7);
    }

    virtual char getBlock(int r, int c) = 0;

    // Thay thế logic boardDelBlock và block2Board
    void updateOnBoard(bool erase = false) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (getBlock(i, j) != ' ') {
                    int ty = y + i;
                    int tx = x + j;
                    if (ty >= 0 && ty < H && tx >= 1 && tx < W - 1) {
                        board[ty][tx] = erase ? ' ' : BLOCK;
                        if (!erase) boardcolor[ty][tx] = color; 
                        else boardcolor[ty][tx] = -1;
                    }
                }
            }
        }
    }

    // Thay thế logic canMove:
    bool canMove(int dx, int dy) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (getBlock(i, j) != ' ') {
                    int tx = x + j + dx;
                    int ty = y + i + dy;
                    if (tx < 1 || tx >= W - 1 || ty >= H - 1) return false;
                    if (ty >= 0 && board[ty][tx] == BLOCK) return false;
                }
            }
        }
        return true;
    }

    // Thay thế logic canRotate
    bool canRotate() {
        int nextRotation = (rotation + 1) % 4;
        int oldRotation = rotation;
        rotation = nextRotation; 
        
        bool possible = true;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (getBlock(i, j) != ' ') {
                    int tx = x + j;
                    int ty = y + i;
                    if (tx < 1 || tx >= W - 1 || ty >= H - 1 || (ty >= 0 && board[ty][tx] == BLOCK)) {
                        possible = false;
                        break;
                    }
                }
            }
        }
        rotation = oldRotation; 
        return possible;
    }

    void rotate() {
        if (canRotate()) {
            rotation = (rotation + 1) % 4;
        }
    }
    
    int getMaxCol() {
        int maxCol = -1;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                // Sử dụng hàm ảo getBlock để lấy dữ liệu của chính nó
                if (this->getBlock(i, j) != ' ') {
                    if (j > maxCol) maxCol = j;
                }
            }
        }
        return maxCol + 1;
    }
};

//Khai báo Block
Block* currentPiece = nullptr; 

// --- CÁC LỚP CON VỚI KHỞI TẠO STATIC BÊN NGOÀI ---

// Block I
class BlockI : public Block {
    static char data[4][4][4];
public:
    BlockI() : Block(12) {} 
    char getBlock(int r, int c) override { return data[rotation][r][c]; }
};
char BlockI::data[4][4][4] = {
    {{' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}},
    {{' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,BLOCK}, {' ',' ',' ',' '}, {' ',' ',' ',' '}},
    {{' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}},
    {{' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,BLOCK}, {' ',' ',' ',' '}, {' ',' ',' ',' '}}
};

// Block O
class BlockO : public Block {
    static char data[4][4][4];
public:
    BlockO() : Block(15) {} 
    char getBlock(int r, int c) override { return data[rotation][r][c]; }
};
char BlockO::data[4][4][4] = {
    {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}}
};

// Block T
class BlockT : public Block {
    static char data[4][4][4];
public:
    BlockT() : Block(14) {}
    char getBlock(int r, int c) override { return data[rotation][r][c]; }
};
char BlockT::data[4][4][4] = {
    {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}},
    {{' ',' ',' ',' '}, {' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,' ',' '}}
};

// Block S
class BlockS : public Block {
    static char data[4][4][4];
public:
    BlockS() : Block(13) {}
    char getBlock(int r, int c) override { return data[rotation][r][c]; }
};
char BlockS::data[4][4][4] = {
    {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {BLOCK,BLOCK,' ',' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',BLOCK,' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {BLOCK,BLOCK,' ',' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',BLOCK,' '}}
};

// Block Z
class BlockZ : public Block {
    static char data[4][4][4];
public:
    BlockZ() : Block(6) {}
    char getBlock(int r, int c) override { return data[rotation][r][c]; }
};
char BlockZ::data[4][4][4] = {
    {{' ',' ',' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',' ',BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}},
    {{' ',' ',' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',' ',BLOCK,' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}}
};

// Block J
class BlockJ : public Block {
    static char data[4][4][4];
public:
    BlockJ() : Block(4) {}
    char getBlock(int r, int c) override { return data[rotation][r][c]; }
};
char BlockJ::data[4][4][4] = {
    {{' ',' ',' ',' '}, {BLOCK,' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,BLOCK,' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}},
    {{' ',' ',' ',' '}, {' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',BLOCK,' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {BLOCK,BLOCK,' ',' '}}
};

// Block L
class BlockL : public Block {
    static char data[4][4][4];
public:
    BlockL() : Block(7) {}
    char getBlock(int r, int c) override { return data[rotation][r][c]; }
};
char BlockL::data[4][4][4] = {
    {{' ',' ',' ',' '}, {' ',' ',BLOCK,' '}, {BLOCK,BLOCK,BLOCK,' '}, {' ',' ',' ',' '}},
    {{' ',' ',' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,BLOCK,' '}},
    {{' ',' ',' ',' '}, {' ',' ',' ',' '}, {BLOCK,BLOCK,BLOCK,' '}, {BLOCK,' ',' ',' '}},
    {{' ',' ',' ',' '}, {BLOCK,BLOCK,' ',' '}, {' ',BLOCK,' ',' '}, {' ',BLOCK,' ',' '}}
};

void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void initBoard(){
    for (int i = 0 ; i < H ; i++){
        for (int j = 0 ; j < W ; j++){
            if(i == H-1) {
                if(j==0) board[i][j] = char(200);
                else if(j==W-1) board[i][j] = char(188);
                else board[i][j] = char(205);
            }
            else if ((j == 0) || (j == W-1))
                board[i][j] = char(186);
            else
                board[i][j] = ' ';
        }
    }
}

void draw() {
    gotoxy(0, 0); 
    int currentColor = -1; // Biến ghi nhớ màu hiện tại của hệ thống

    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            int pixelColor = 7; // Màu mặc định (Trắng xám)

            // 1. Xác định màu của ô (pixel) này
            if (board[i][j] == BLOCK) {
                // Nếu là khối đã cố định thì lấy màu trong mảng, nếu đang rơi thì lấy màu currentPiece
                pixelColor = (boardcolor[i][j] != -1) ? boardcolor[i][j] : currentPiece->getColor();
            } 
            else if (board[i][j] != ' ') {
                // Nếu là KHUNG: Lấy màu của khối đang rơi 
                if (currentPiece != nullptr) pixelColor = currentPiece->getColor();
            }

            // 2. TỐI ƯU: Chỉ gọi hàm đổi màu nếu màu ô này KHÁC màu ô trước đó
            if (pixelColor != currentColor) {
                SetConsoleTextAttribute(hConsole, pixelColor);
                currentColor = pixelColor;
            }

            // 3. In ô đó ra
            if (board[i][j] == ' ') cout << "  ";
            else cout << board[i][j] << board[i][j];
        }
        cout << "\n";
    }
    
    // Cuối cùng reset về màu trắng cho phần chữ Score
    //SetConsoleTextAttribute(hConsole, 7);
    cout << "Score: " << score << "        Next: ";
    char nextBlockName[] = {'I', 'O', 'T', 'S', 'Z', 'J', 'L'};
    if (nextBlock >= 0 && nextBlock < 7) {
        cout << nextBlockName[nextBlock];
    }
    cout << "\n";
    cout << "Controls: A/D=Move  S=Down  W=Rotate  Q=Quit  Space=Hard drop\n";
    cout.flush();
}

void removeLine(){
    for (int i = H - 2; i >= 1; i--){
        bool full = true;
        for (int j = 1; j < W - 1; j++){
            if (board[i][j] != BLOCK) {
                full = false;
                break;
            }
        }
        if (full) {
            for (int k = i; k > 0; k--){
                for (int j = 1; j < W - 1; j++){
                    board[k][j] = board[k - 1][j];
                    boardcolor[k][j] = boardcolor[k - 1][j];
                }
            }
            for (int j = 1; j < W - 1; j++){
                board[0][j] = ' ';
            }
            score += 100;
            i++;  
        }
    }
}

void setUpBoardColor() {
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            boardcolor[i][j] = -1;
        }
    }
}

int getRandomX(Block* p) {
    if (!p) return 4;
    int blockMaxCol = p->getMaxCol();
    int maxX = W - 1 - blockMaxCol;
    if (maxX < 1) maxX = 1;
    return 1 + (rand() % maxX);
}

bool isGameOver() {
    for (int j = 1; j < W - 1; j++) {
        if (board[0][j] == BLOCK) {
            return true;
        }
    }
    return false;
}

Block* spawnBlock(int type) {
    switch (type) {
        case 0: return new BlockI();
        case 1: return new BlockO();
        case 2: return new BlockT();
        case 3: return new BlockS();
        case 4: return new BlockZ();
        case 5: return new BlockJ();
        case 6: return new BlockL();
        default: return new BlockI();
    }
}

int main() {
    system("chcp 437 >nul");
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hOut, &cursorInfo);
    cursorInfo.bVisible = FALSE; // Ẩn con trỏ đi
    SetConsoleCursorInfo(hOut, &cursorInfo);

    srand((unsigned)time(0));
    initBoard();
    setUpBoardColor();
    nextBlock = rand() % 7;
    int b =  nextBlock;
    currentPiece = spawnBlock(b);
    currentPiece->setPos(getRandomX(currentPiece), 0); // Đặt vị trí X ngẫu nhiên ngay khi tạo
    nextBlock = rand() % 7;
    int fallCounter = 0;
    bool gameOver = false;
    while (!gameOver){
        currentPiece->updateOnBoard(true);
        if (_kbhit()){
            unsigned char ch = _getch();
            char c = tolower(ch);
            if (c == 'a') {
                if (currentPiece->canMove(-1, 0)) {
                    currentPiece->setPos(currentPiece->getX() - 1, currentPiece->getY());
                }
            }
            else if (c == 'd') {
                if (currentPiece->canMove(1, 0)) {
                    currentPiece->setPos(currentPiece->getX() + 1, currentPiece->getY());
                }
            }
            else if (c == 's') {
                if (currentPiece->canMove(0, 1)) {
                    currentPiece->setPos(currentPiece->getX(), currentPiece->getY() + 1);
                }
            }
            else if (c == 'w') {
                currentPiece->rotate();;
            }
            else if (c == 32) {
                while (currentPiece->canMove(0, 1)) {
                    currentPiece->setPos(currentPiece->getX(), currentPiece->getY() + 1);
                }
            }
            else if (c == 'q') {
                gameOver = true;
                break;
            }
        }
        fallCounter++;
        if (fallCounter >= times) {
            if (currentPiece->canMove(0, 1)) {
                currentPiece->setPos(currentPiece->getX(), currentPiece->getY() + 1);
            }
            else {
                currentPiece->updateOnBoard(false);
                removeLine();
                if (isGameOver()) {
                    draw();
                    cout << "\n========== GAME OVER ==========" << endl;
                    cout << "Final Score: " << score << endl;
                    cout << "================================" << endl;
                    cout << "Press any key to exit..." << endl;
                    _getch();
                    gameOver = true;
                    break;
                }
                delete currentPiece; 
                b = nextBlock; // Giữ lại biến b để hàm draw() biết tên khối tiếp theo
                currentPiece = spawnBlock(b);
                currentPiece->setPos(getRandomX(currentPiece), 0);
                nextBlock = rand() % 7;
                if (!currentPiece->canMove(0, 0)) {
                    draw();
                    cout << "\n========== GAME OVER ==========" << endl;
                    cout << "Final Score: " << score << endl;
                    cout << "================================" << endl;
                    cout << "Press any key to exit..." << endl;
                    _getch();
                    gameOver = true;
                    break;
                }
                fallCounter = 0;
            }
            fallCounter = 0;
        }
        currentPiece->updateOnBoard(false);
        draw();
        Sleep(30);  
    }
    return 0;
}