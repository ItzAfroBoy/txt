/*
* txt - A shitty text editor
* Copyright (C) 2021 ItzAfroBoy
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <termios.h>
#include <time.h>

#define ABUF_INIT {NULL, 0}
#define CTRL_KEY(k) ((k)&0x1f)
#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)

static char *C_HL_extensions[] = {".c", ".h", ".cpp", NULL};
static char *C_HL_keywords[] = {"switch", "default", "if", "else", "while", "for", "break", "continue", "return", "struct|", "union|", "typedef", "static", "class|", "case", "enum|", "int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|", "#define", "#include", "#if", "#ifndef", "#endif", "ifdef", "#defined", "#else", "#elif", "#error", "#line", NULL};

static char *PY_HL_extensions[] = {".py", ".py3", ".pyw", NULL};
static char *PY_HL_keywords[] = {"if", "else", "for", "in", "is", "pass", "elif", "def|", "not", "async|", "import|", NULL};

enum editorKey
{
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

enum editorHighlight
{
    HL_NORMAL = 0,
    HL_COMMENT,
    HL_MLCOMMENT,
    HL_KEYWORD1,
    HL_KEYWORD2,
    HL_STRING,
    HL_NUMBER,
    HL_MATCH
};

typedef struct erow
{
    int idx;
    int size;
    int rsize;
    int hl_open_comment;
    char *chars;
    char *render;
    unsigned char *hl;
} erow;

struct abuf
{
    char *b;
    int len;
};

struct editorConfig
{
    char *filename;
    char statusmsg[80];
    int numrows;
    int screenrows;
    int screencols;
    int coloff;
    int rowoff;
    int dirty;
    int cx, cy;
    int rx;
    erow *row;
    struct editorSyntax *syntax;
    struct termios orig_termios;
    time_t statusmsg_time;
} E;

struct editorSyntax
{
    char *filetype;
    char **filematch;
    char **keywords;
    char *singleline_comment_start;
    char *multiline_comment_start;
    char *multiline_comment_end;
    int flags;
};

static struct editorSyntax HLDB[] = {
    {   
        "c",
        C_HL_extensions,
        C_HL_keywords,
        "//", "/*", "*/",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    },
    {
        "py",
        PY_HL_extensions,
        PY_HL_keywords,
        "#", "\"\"\"", "\"\"\"",
        HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
    }
};

void disableRawMode(void);
void enableRawMode(void);

void die(const char *s);
int is_separator(int c);

void showHelp(void);
void showVersion(void);

int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);

void editorUpdateSyntax(erow *row);
void editorSelectSyntaxHighlight(void);
int editorSyntaxToColor(int hl);

int editorRowCxToRx(erow *row, int cx);
int editorRowRxToCx(erow *row, int rx);

void editorDelChar(void);
void editorDelRow(int at);

void editorFreeRow(erow *row);
void editorUpdateRow(erow *row);

void editorRowInsertChar(erow *row, int at, int c);
void editorRowAppendString(erow *row, char *s, size_t len);

void editorInsertRow(int at, char *s, size_t len);
void editorInsertChar(int c);
void editorInsertNewline(void);

void initEditor(void);
void editorSave(void);
void editorFind(void);
void editorScroll(void);
void editorOpen(char *filename);

void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);

void editorDrawRows(struct abuf *ab);
void editorDrawStatusBar(struct abuf *ab);
void editorDrawMessageBar(struct abuf *ab);

int editorReadKey(void);
void editorRefreshScreen(void);
void editorProcessKeypress(void);
void editorMoveCursor(int key);
void editorSetStatusMessage(const char *fmt, ...);

char *editorRowsToString(int *buflen);
char *editorPrompt(char *prompt, void (*callback)(char *, int));
void editorFindCallback(char *query, int key);
