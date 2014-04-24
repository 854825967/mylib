#if defined WIN32 || defined WIN64

#ifndef _CONSOLE_H
#define _CONSOLE_H

#include "MultiSys.h"
#include "CLock.h"
using namespace tlib;

// CConsole
// 封装控制台的输入输出功能类
const u32 FORECOLOR_RED = FOREGROUND_RED;
const u32 FORECOLOR_GREEN = FOREGROUND_GREEN;
const u32 FORECOLOR_BLUE = FOREGROUND_BLUE;
const u32 FORECOLOR_LIGHTRED = FOREGROUND_RED | FOREGROUND_INTENSITY;
const u32 FORECOLOR_LIGHTGREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const u32 FORECOLOR_LIGHTBLUE = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
const u32 FORECOLOR_GRAY = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
const u32 FORECOLOR_WHITE = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const u32 FORECOLOR_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN;
const u32 FORECOLOR_LIGHTYELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

class CConsole  
{
public:
    CConsole(size_t height, size_t width, size_t logheight, 
        u32 color = FORECOLOR_LIGHTGREEN);
    ~CConsole();

    CLockUnit * GetLock();
    void SetSize(size_t height, size_t width, size_t logheight);
    void SetColor(u32 color);
    void Initialize();
    void Shutdown();
    void Clear();
    void WriteLogString(const char * str, u32 color = FORECOLOR_LIGHTGREEN);
    void WriteOutString(const char * str, u32 color = FORECOLOR_LIGHTGREEN);
    void WritePrompt(const char * str);
    void DoPrompt(char * strPrompt, char * strBuffer, size_t maxlen);
    bool ReadChar(char & value);

private:
    CConsole();

    void WriteLine(size_t line, const char * string, 
        u32 color = FORECOLOR_LIGHTGREEN);
    size_t GetCmdRow() const;

private:
    CLockUnit    m_Lock;
    HANDLE        m_hStdOut;
    HANDLE        m_hStdIn;
    bool        m_bLoaded;
    size_t        m_nHeight;
    size_t        m_nWidth;
    size_t        m_nLogHeight;
    u32        m_nForeColor;
};

#endif // _CONSOLE_H

#endif //#if defined WIN32 || defined WIN64
