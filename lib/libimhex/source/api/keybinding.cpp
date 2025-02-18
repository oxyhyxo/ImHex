#include <hex/api/keybinding.hpp>
#include <imgui.h>

#include <hex/ui/view.hpp>

namespace hex {

    std::map<Shortcut, std::function<void()>> ShortcutManager::s_globalShortcuts;

    void ShortcutManager::addGlobalShortcut(const Shortcut &shortcut, const std::function<void()> &callback) {
        ShortcutManager::s_globalShortcuts.insert({ shortcut, callback });
    }

    void ShortcutManager::addShortcut(View *view, const Shortcut &shortcut, const std::function<void()> &callback) {
        view->m_shortcuts.insert({ shortcut + CurrentView, callback });
    }

    static Shortcut getShortcut(bool ctrl, bool alt, bool shift, bool super, bool focused, u32 keyCode) {
        Shortcut pressedShortcut;

        if (ctrl)
            pressedShortcut += CTRL;
        if (alt)
            pressedShortcut += ALT;
        if (shift)
            pressedShortcut += SHIFT;
        if (super)
            pressedShortcut += SUPER;
        if (focused)
            pressedShortcut += CurrentView;

        pressedShortcut += static_cast<Keys>(keyCode);

        return pressedShortcut;
    }

    void ShortcutManager::process(const std::unique_ptr<View> &currentView, bool ctrl, bool alt, bool shift, bool super, bool focused, u32 keyCode) {
        Shortcut pressedShortcut = getShortcut(ctrl, alt, shift, super, focused, keyCode);

        if (currentView->m_shortcuts.contains(pressedShortcut + AllowWhileTyping)) {
            currentView->m_shortcuts[pressedShortcut + AllowWhileTyping]();
        } else if (currentView->m_shortcuts.contains(pressedShortcut)) {
            if (!ImGui::GetIO().WantTextInput)
                currentView->m_shortcuts[pressedShortcut]();
        }
    }

    void ShortcutManager::processGlobals(bool ctrl, bool alt, bool shift, bool super, u32 keyCode) {
        Shortcut pressedShortcut = getShortcut(ctrl, alt, shift, super, false, keyCode);

        if (ShortcutManager::s_globalShortcuts.contains(pressedShortcut + AllowWhileTyping)) {
            ShortcutManager::s_globalShortcuts[pressedShortcut + AllowWhileTyping]();
        } else if (ShortcutManager::s_globalShortcuts.contains(pressedShortcut)) {
            if (!ImGui::GetIO().WantTextInput)
                ShortcutManager::s_globalShortcuts[pressedShortcut]();
        }
    }

    void ShortcutManager::clearShortcuts() {
        ShortcutManager::s_globalShortcuts.clear();
    }

}