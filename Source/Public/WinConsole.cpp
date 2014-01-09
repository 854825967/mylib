#if defined WIN32 || defined WIN64

#include "WinConsole.h"
#include "WinTools.h"
// CConsole

static BOOL WINAPI ConsoleCtrlHandler(DWORD type)
{
    switch (type)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        return TRUE;
    }

    return FALSE;
}

CConsole::CConsole(size_t height, size_t width, size_t logheight, u32 color)
{
    m_hStdOut = NULL;
    m_hStdIn = NULL;
    m_bLoaded = false;
    m_nHeight = height;
    m_nWidth = width;
    m_nLogHeight = logheight;
    m_nForeColor = color;
}

CConsole::~CConsole()
{
    if (m_bLoaded)
    {
        Shutdown();
    }
}

size_t CConsole::GetCmdRow() const
{
    return m_nHeight - 2;
}

CLockUnit * CConsole::GetLock()
{
    return &m_Lock;
}

void CConsole::SetSize(size_t height, size_t width, size_t logheight)
{
    ASSERT((height >= 10) && (height < 100));
    ASSERT((width >= 10) && (width < 256));
    ASSERT((logheight >= 2) && (logheight < height));

    COORD        coord = { 0, 0 };
    SMALL_RECT    rect;

    m_nHeight = height;
    m_nWidth = width;
    m_nLogHeight = logheight;

    if (m_bLoaded)
    {
        rect.Left = 0;
        rect.Top = 0;
        rect.Right = SHORT(width - 1);
        rect.Bottom = SHORT(height - 1);

        ::SetConsoleWindowInfo(m_hStdOut, FALSE, &rect);

        coord.X = SHORT(width);
        coord.Y = SHORT(height);

        ::SetConsoleScreenBufferSize(m_hStdOut, coord);

        Clear();
    }
}

void CConsole::SetColor(u32 color)
{
    m_nForeColor = color;

    if (m_bLoaded)
    {
        ::SetConsoleTextAttribute(m_hStdOut, m_nForeColor);
    }
}

void CConsole::Initialize()
{
    if (m_bLoaded)
    {
        return;
    }

    m_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    m_hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    ::SetThreadExecutionState(
        ES_SYSTEM_REQUIRED | ES_CONTINUOUS | ES_DISPLAY_REQUIRED); // suspend power management
    ::SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
    ::SetConsoleMode(m_hStdIn, 0);
    ::SetConsoleMode(m_hStdOut, 0);

    m_bLoaded = true;

    SetColor(m_nForeColor);
    SetSize(m_nHeight, m_nWidth, m_nLogHeight);
}

void CConsole::Shutdown()
{
    if (! m_bLoaded)
    {
        return;
    }

    m_bLoaded = false;
}

void CConsole::Clear()
{
    static char strEmpty[255] =
        "                                                                   "
        "                                                                   "
        "                                                                   "
        "                                                     ";
    static char strSeper[255] =
        "==================================================================="
        "==================================================================="
        "==================================================================="
        "=====================================================";

    //ASSERT(m_bLoaded);

    if (! m_bLoaded)
    {
        return;
    }

    DWORD dwWritten;
    COORD coord = { 0, 0 };

    for (size_t i=0; i<m_nHeight; i++)
    {
        coord.Y = (WORD)i;

        ::SetConsoleCursorPosition(m_hStdOut, coord);

        if (i == (m_nLogHeight - 1))
        {
            ::WriteConsole(m_hStdOut, strSeper, DWORD(m_nWidth), &dwWritten, NULL);
        }
        else
        {
            ::WriteConsole(m_hStdOut, strEmpty, DWORD(m_nWidth), &dwWritten, NULL);
        }
    }
}

void CConsole::WriteLine(size_t line, const char * str, u32 color)
{
    ASSERT(str != NULL);

    if (::strlen(str) >= 512)
    {
        return;
    }

    char    data[600];
    WORD    attr[600];
    DWORD    dwWritten;
    COORD    coord = { 0, (WORD)line };

    ::CopyString(data, sizeof(data), str);

    size_t i;

    for (i=::strlen(str); i<m_nWidth; i++)
    {
        data[i] = ' ';
    }

    data[m_nWidth] = 0;

    for (i=0; i<m_nWidth; i++)
    {
        attr[i] = color;
    }

    ::WriteConsoleOutputAttribute(m_hStdOut,
        attr, DWORD(m_nWidth), coord, &dwWritten);
    ::WriteConsoleOutputCharacter(m_hStdOut,
        data, DWORD(m_nWidth), coord, &dwWritten);
}

void CConsole::WriteLogString(const char * str, u32 color)
{
    //ASSERT(m_bLoaded);
    ASSERT(m_nLogHeight >= 2);

    if (! m_bLoaded)
    {
        return;
    }

    if (::strlen(str) >= 500)
    {
        return;
    }

    char        data[600];
    SMALL_RECT    rect;
    COORD        coord = { 0, 0 };
    CHAR_INFO    ciFill;

    // scroll log information
    rect.Left = 0;
    rect.Top = 1;
    rect.Right = SHORT(m_nWidth - 1);
    rect.Bottom = SHORT(m_nLogHeight - 2);

    ciFill.Char.AsciiChar = ' ';
    ciFill.Attributes = 0;

    ::ScrollConsoleScreenBuffer(m_hStdOut, &rect, NULL, coord, &ciFill);

    SYSTEMTIME tm;

    ::GetLocalTime(&tm);

    ::SafeSprintf(data, sizeof(data),
        "[%04d-%02d-%02d %02d:%02d:%02d] %s",
        tm.wYear, tm.wMonth, tm.wDay,
        tm.wHour, tm.wMinute, tm.wSecond, str);

    WriteLine(m_nLogHeight - 2, data, color);
}

void CConsole::WriteOutString(const char * str, u32 color)
{
    //ASSERT(m_bLoaded);

    if (! m_bLoaded)
    {
        return;
    }

    SMALL_RECT    rect;
    COORD        coord = { 0, (WORD)m_nLogHeight };
    CHAR_INFO    ciFill;

    // scroll output
    rect.Left = 0;
    rect.Top = SHORT(m_nLogHeight + 1);
    rect.Right = SHORT(m_nWidth - 1);
    rect.Bottom = SHORT(GetCmdRow() - 2);

    ciFill.Char.AsciiChar = ' ';
    ciFill.Attributes = 0;

    ::ScrollConsoleScreenBuffer(m_hStdOut, &rect, NULL, coord, &ciFill);

    WriteLine(GetCmdRow() - 2, str, color);
}

void CConsole::WritePrompt(const char * str)
{
    ASSERT(str != NULL);

    WriteLine(GetCmdRow(), str, m_nForeColor);

    COORD coord;

    coord.X = SHORT(::strlen(str));
    coord.Y = SHORT(GetCmdRow());

    ::SetConsoleCursorPosition(m_hStdOut, coord);
}

void CConsole::DoPrompt(char * strPrompt, char * strBuffer, size_t maxlen)
{
    ASSERT(m_bLoaded);
    ASSERT(strPrompt != NULL);
    ASSERT(strBuffer != NULL);

    char    data[512];
    DWORD    dwRead;
    COORD    coord = { 0, 0 };
    bool    ended = false;
    size_t    count = 0;

    WriteLine(GetCmdRow(), strPrompt, m_nForeColor);

    coord.X = SHORT(::strlen(strPrompt));
    coord.Y = SHORT(GetCmdRow());

    ::SetConsoleCursorPosition(m_hStdOut, coord);

    while (! ended)
    {
        if (::ReadConsole(m_hStdIn, data, 128, &dwRead, NULL))
        {
            for (size_t i=0; i<dwRead; i++)
            {
                if ('\r' == data[i])
                {
                    ended = true;
                }
                else if ('\b' == data[i])
                {
                    if (count > 0)
                    {
                        count --;
                    }
                }
                else
                {
                    if (count < (maxlen - 1))
                    {
                        strBuffer[count++] = data[i];
                    }
                }
            }

            strBuffer[count] = 0;

            ::SafeSprintf(data, sizeof(data),
                "%s%s", strPrompt, strBuffer);

            WriteLine(GetCmdRow(), data, m_nForeColor);

            coord.X = SHORT(::strlen(data));
            coord.Y = SHORT(GetCmdRow());

            ::SetConsoleCursorPosition(m_hStdOut, coord);
        }
    }

    WriteOutString(data, m_nForeColor);
}

bool CConsole::ReadChar(char & value)
{
    ASSERT(m_bLoaded);

    DWORD events;

    if (! ::GetNumberOfConsoleInputEvents(m_hStdIn, &events))
    {
        return false;
    }

    if (0 == events)
    {
        return false;
    }

    INPUT_RECORD rec;

    if (! ::ReadConsoleInput(m_hStdIn, &rec, 1, &events))
    {
        return false;
    }

    if (0 == events)
    {
        return false;
    }

    if (rec.EventType != KEY_EVENT)
    {
        return false;
    }

    if (! rec.Event.KeyEvent.bKeyDown)
    {
        return false;
    }

    value = rec.Event.KeyEvent.uChar.AsciiChar;

    return (value != 0);
}

#endif// defined WIN32 || defined WIN64
