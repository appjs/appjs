#ifdef WIN32
#ifndef CLIPBOARD_WIN
#define CLIPBOARD_WIN

#include <Windows.h>
#include <functional>

namespace appjs {
class ClipboardImplW32
{
public:
    typedef std::function<void ()> slot_t;
    ClipboardImplW32(slot_t slot);
    ~ClipboardImplW32(void);

    slot_t slot_; 

private:
    HWND handle_;

/* Not to be implemented */
private:
    ClipboardImplW32 (const ClipboardImplW32&);
    ClipboardImplW32& operator= (const ClipboardImplW32&);
};
}

#endif
#endif
