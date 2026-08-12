#pragma once
//
// GlobalHotkeyManager — intercepts system-wide hotkeys via a low-level
// keyboard hook (WH_KEYBOARD_LL), allowing PinIt to override reserved
// Windows shortcuts like Win+T that RegisterHotKey cannot claim.
//
#include <QObject>
#include <QHash>

#include <windows.h>

#include "persistence.h"

class GlobalHotkeyManager : public QObject
{
    Q_OBJECT
public:
    explicit GlobalHotkeyManager(QObject *parent = nullptr);
    ~GlobalHotkeyManager() override;

    // Register the four PinIt shortcuts. Returns false only if the hook
    // could not be installed; partial failures are reported via failedActions().
    bool registerAll(const persistence::ShortcutConfig &config);
    void unregisterAll();

    QStringList failedActions() const { return m_failed; }

signals:
    void togglePin();
    void opacityUp();
    void opacityDown();
    void toggleWindow();

private:
    // One parsed shortcut entry for matching in the hook callback.
    struct HotkeyEntry {
        unsigned mods;   // MOD_* flags
        unsigned vk;     // virtual-key code
    };

    // Internal: install/remove the low-level keyboard hook.
    bool installHook();
    void removeHook();

    // The hook callback — a static function that forwards to the singleton.
    static LRESULT CALLBACK keyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    // Process one keystroke from the hook. Returns true if the key was consumed.
    bool processKey(WPARAM wParam, unsigned vk);

    // Check if the current modifier state matches the given shortcut.
    bool matchModifiers(unsigned requiredMods) const;

    // Singleton pointer for the static callback.
    static GlobalHotkeyManager *s_instance;

    HHOOK m_hook = nullptr;
    QHash<int, HotkeyEntry> m_hotkeys;  // id -> parsed shortcut
    QStringList m_failed;
    bool m_anyRegistered = false;
};
