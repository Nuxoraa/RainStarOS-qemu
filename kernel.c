// --- Настройки системы ---
#define VIDEO_MEM_ADDR 0xB8000
#define COLS 80
#define ROWS 25
#define MAX_FILES 16
#define MAX_FILE_SIZE 1024

// --- Структуры ---
typedef struct {
    char name[32];
    char content[MAX_FILE_SIZE];
    int active;
} File;

typedef struct {
    char name[32];
    char content[MAX_FILE_SIZE];
    char desc[64];
} Package;

// --- Глобальные переменные ---
char* video = (char*)VIDEO_MEM_ADDR;
int cursor_x = 0, cursor_y = 0;
int shift_pressed = 0;
int ctrl_pressed = 0;
File fs[MAX_FILES];

// --- Репозиторий пакетов (pakj) ---
Package repo[] = {
    {"calc.bas", "PRINT 2 + 2 = 4\nPRINT MATH OK", "Simple Calculator"},
    {"matrix.bas", "PRINT 110101\nPRINT 001101\nPRINT 101011", "Matrix Animation"},
    {"about.txt", "Obsidian OS v2.0\nCreated by nuxora", "OS Information"}
};
#define REPO_SIZE 3

// --- Низкоуровневые функции ---
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(unsigned short port, unsigned char val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// --- Утилиты строк ---
int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return *a - *b;
}

int starts_with(const char* str, const char* pref) {
    while (*pref) { if (*pref != *str) return 0; pref++; str++; }
    return 1;
}

void strcpy(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

// --- Видео и Печать ---
void update_cursor(int x, int y) {
    unsigned short pos = y * COLS + x;
    outb(0x3D4, 0x0F); outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E); outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void print_char_ex(char c, unsigned char color) {
    if (c == '\n') { cursor_x = 0; cursor_y++; }
    else if (c == '\b') { if (cursor_x > 0) { cursor_x--; video[(cursor_y * COLS + cursor_x) * 2] = ' '; } }
    else {
        video[(cursor_y * COLS + cursor_x) * 2] = c;
        video[(cursor_y * COLS + cursor_x) * 2 + 1] = color;
        cursor_x++;
    }
    if (cursor_x >= COLS) { cursor_x = 0; cursor_y++; }
    if (cursor_y >= ROWS) cursor_y = 0; 
    update_cursor(cursor_x, cursor_y);
}

void print(const char* s) { while (*s) print_char_ex(*s++, 0x07); }

void clear() {
    for (int i = 0; i < COLS * ROWS * 2; i += 2) { video[i] = ' '; video[i+1] = 0x07; }
    cursor_x = 0; cursor_y = 0; update_cursor(0, 0);
}

// --- Команды ---

void run_fastool() {
    unsigned int ebx, ecx, edx, unused;
    char vendor[13];
    asm volatile("cpuid" : "=a"(unused), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(0));
    *((int*)(vendor)) = ebx; *((int*)(vendor + 4)) = edx; *((int*)(vendor + 8)) = ecx; vendor[12] = '\0';

    print("\n  "); print_char_ex(219, 0x0D); print_char_ex(223, 0x0D); print("  nuxora@obsidian\n");
    print("  OS:     Obsidian OS v2.0\n");
    print("  CPU:    "); print(vendor); print("\n");
    print("  RAM:    64 MB\n");
}

void run_vbasic(const char* code) {
    int p = 0; print("\n--- VBasic ---\n");
    while(code[p]) {
        if(starts_with(&code[p], "PRINT ")) {
            p += 6; while(code[p] != '\n' && code[p]) print_char_ex(code[p++], 0x0B);
            print("\n");
        }
        while(code[p] != '\n' && code[p]) p++;
        if(code[p] == '\n') p++;
    }
}

// --- Клавиатура ---
unsigned char kbd_map[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=','\b','\t',
    'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' '
};

unsigned char kbd_shift_map[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+','\b','\t',
    'Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' '
};

// --- Основное ядро ---
void kernel_main() {
    for(int i=0; i<MAX_FILES; i++) fs[i].active = 0;
    clear();
    print("Welcome to Obsidian. Type 'dnf install calc.bas' to start.\nOS> ");
    char buf[80]; int idx = 0;

    while(1) {
        if(inb(0x64) & 1) {
            unsigned char sc = inb(0x60);
            if(sc == 0x2A || sc == 0x36) shift_pressed = 1;
            else if(sc == 0xAA || sc == 0xB6) shift_pressed = 0;
            else if(!(sc & 0x80)) {
                char c = shift_pressed ? kbd_shift_map[sc] : kbd_map[sc];
                if(c == '\n') {
                    buf[idx] = 0; print("\n");
                    if(strcmp(buf, "fastool") == 0) run_fastool();
                    else if(strcmp(buf, "ls") == 0) {
                        for(int i=0; i<MAX_FILES; i++) if(fs[i].active) { print(fs[i].name); print(" "); }
                    }
                    else if(starts_with(buf, "dnf install ")) {
                        for(int i=0; i<REPO_SIZE; i++) if(strcmp(repo[i].name, buf+12) == 0) {
                            for(int j=0; j<MAX_FILES; j++) if(!fs[j].active) {
                                strcpy(fs[j].name, repo[i].name); strcpy(fs[j].content, repo[i].content);
                                fs[j].active = 1; print("Installed!"); break;
                            }
                        }
                    }
                    else if(starts_with(buf, "vbasic ")) {
                        for(int i=0; i<MAX_FILES; i++) if(fs[i].active && strcmp(fs[i].name, buf+7)==0) run_vbasic(fs[i].content);
                    }
                    else if(strcmp(buf, "clear") == 0) clear();
                    print("\nOS> "); idx = 0;
                } else if(c == '\b' && idx > 0) { idx--; print_char_ex('\b', 0x07); }
                else if(c && idx < 79) { buf[idx++] = c; print_char_ex(c, 0x07); }
            }
        }
    }
}
