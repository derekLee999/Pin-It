#include "globalhotkey.h"
#include "shortcuts.h"

#include <windows.h>

// Hotkey ids — used as keys in the m_hotkeys hash and to dispatch signals.
enum HotkeyId {
    IdTogglePin    = 1,
    IdOpacityUp    = 2,
    IdOpacityDown  = 3,
    IdToggleWindow = 4,
};

// Singleton pointer so the static hook callback can reach the instance.
GlobalHotkeyManager *GlobalHotkeyManager::s_instance = nullptr;

GlobalHotkeyManager::GlobalHotkeyManager(QObject *parent)
    : QObject(parent)
{
}

GlobalHotkeyManager::~GlobalHotkeyManager()
{
    unregisterAll();
}

bool GlobalHotkeyManager::registerAll(const persistence::ShortcutConfig &c)
{
    unregisterAll();
    m_failed.clear();
    m_anyRegistered = false;

    struct Entry { int id; const char *label; QString shortcut; };
    const Entry entries[] = {
        { IdTogglePin,    "Pin/Unpin", c.togglePin },
        { IdOpacityUp,    "Opacity +", c.opacityUp },
        { IdOpacityDown,  "Opacity -", c.opacityDown },
        { IdToggleWindow, "Show/Hide", c.toggleWindow },
    };

    for (const Entry &e : entries) {
        unsigned mods = 0, vk = 0;
        if (shortcuts::parse(e.shortcut, mods, vk)) {
            m_hotkeys.insert(e.id, {mods, vk});
            m_anyRegistered = true;
        } else {
            m_failed << QString::fromLatin1(e.label);
        }
    }

    if (m_anyRegistered && !installHook()) {
        m_failed.clear();
        m_failed << QStringLiteral("Keyboard hook");
        m_anyRegistered = false;
    }

    return m_anyRegistered;
}

void GlobalHotkeyManager::unregisterAll()
{
    removeHook();
    m_hotkeys.clear();
    m_anyRegistered = false;
}

// --- Low-level keyboard hook -----------------------------------------------

bool GlobalHotkeyManager::installHook()
{
    if (m_hook)
        return true;

    s_instance = this;
    m_hook = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardHookProc,
                               GetModuleHandleW(nullptr), 0);
    if (!m_hook) {
        s_instance = nullptr;
        qWarning("GlobalHotkeyManager: SetWindowsHookExW failed (error %lu)",
                 GetLastError());
    }
    return m_hook != nullptr;
}

void GlobalHotkeyManager::removeHook()
{
    if (m_hook) {
        UnhookWindowsHookEx(m_hook);
        m_hook = nullptr;
    }
    if (s_instance == this)
        s_instance = nullptr;
}

bool GlobalHotkeyManager::matchModifiers(unsigned requiredMods) const
{
    // GetAsyncKeyState returns the live state of each modifier, which is
    // more reliable than tracking key-up/key-down in the hook because we
    // might miss events if the app starts while a modifier is already held.
    bool win   = (GetAsyncKeyState(VK_LWIN)   & 0x8000) ||
                 (GetAsyncKeyState(VK_RWIN)   & 0x8000);
    bool ctrl  = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ||
                 (GetAsyncKeyState(VK_RCONTROL) & 0x8000);
    bool alt   = (GetAsyncKeyState(VK_LMENU)  & 0x8000) ||
                 (GetAsyncKeyState(VK_RMENU)  & 0x8000);
    bool shift = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ||
                 (GetAsyncKeyState(VK_RSHIFT) & 0x8000);

    if ((requiredMods & MOD_WIN)     && !win)   return false;
    if ((requiredMods & MOD_CONTROL) && !ctrl)  return false;
    if ((requiredMods & MOD_ALT)     && !alt)   return false;
    if ((requiredMods & MOD_SHIFT)   && !shift) return false;

    // Ensure no EXTRA modifiers are held (e.g. user pressed Win+Ctrl+Shift+T
    // but the shortcut is only Win+Ctrl+T).
    if (!(requiredMods & MOD_WIN)     && win)   return false;
    if (!(requiredMods & MOD_CONTROL) && ctrl)  return false;
    if (!(requiredMods & MOD_ALT)     && alt)   return false;
    if (!(requiredMods & MOD_SHIFT)   && shift) return false;

    return true;
}

bool GlobalHotkeyManager::processKey(WPARAM wParam, unsigned vk)
{
    // Only act on key-down (not key-up, not repeat).
    if (wParam != WM_KEYDOWN && wParam != WM_SYSKEYDOWN)
        return false;

    for (auto it = m_hotkeys.cbegin(); it != m_hotkeys.cend(); ++it) {
        if (it->vk == vk && matchModifiers(it->mods)) {
            // Emit the corresponding signal.
            switch (it.key()) {
            case IdTogglePin:    emit togglePin();    break;
            case IdOpacityUp:    emit opacityUp();    break;
            case IdOpacityDown:  emit opacityDown();  break;
            case IdToggleWindow: emit toggleWindow(); break;
            }
            return true;   // consume the key — system never sees it
        }
    }
    return false;
}

LRESULT CALLBACK GlobalHotkeyManager::keyboardHookProc(int nCode, WPARAM wParam,
                                                        LPARAM lParam)
{
    if (nCode >= 0 && s_instance) {
        const KBDLLHOOKSTRUCT *kb = reinterpret_cast<const KBDLLHOOKSTRUCT *>(lParam);
        unsigned vk = kb->vkCode;

        // Ignore injected events to avoid feedback loops.
        if (!(kb->flags & LLKHF_INJECTED)) {
            if (s_instance->processKey(wParam, vk))
                return 1;   // swallow the key
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
