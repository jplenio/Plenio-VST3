#include "platform/dialogs.h"
#include "core/paths.h"

#ifdef _WIN32
 #include <windows.h>
 #include <shobjidl.h>
 #include <shellapi.h>
 #include <shlobj.h>
#endif

namespace plenio::platform
{
#ifdef _WIN32
namespace
{
template <typename T>
struct Com
{
    T* p = nullptr;
    ~Com()
    {
        if (p != nullptr)
            p->Release();
    }
    T** put() { return &p; }
    T* operator->() const { return p; }
};

std::optional<std::filesystem::path> showDialog (bool folders, const std::string& title, const std::vector<FileFilter>& filters, void* owner)
{
    Com<IFileOpenDialog> dialog;
    if (FAILED (CoCreateInstance (CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS (dialog.put()))))
        return std::nullopt;
    DWORD options = 0;
    dialog->GetOptions (&options);
    options |= FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST;
    if (folders)
        options |= FOS_PICKFOLDERS;
    else
        options |= FOS_FILEMUSTEXIST;
    dialog->SetOptions (options);
    auto wideTitle = paths::fromUtf8 (title).wstring();
    dialog->SetTitle (wideTitle.c_str());

    std::vector<std::wstring> texts;
    std::vector<COMDLG_FILTERSPEC> specs;
    texts.reserve (filters.size() * 2);
    for (const auto& f : filters)
    {
        texts.push_back (paths::fromUtf8 (f.label).wstring());
        texts.push_back (paths::fromUtf8 (f.patterns).wstring());
    }
    for (std::size_t i = 0; i < filters.size(); ++i)
        specs.push_back ({ texts[i * 2].c_str(), texts[i * 2 + 1].c_str() });
    if (! folders && ! specs.empty())
        dialog->SetFileTypes (static_cast<UINT> (specs.size()), specs.data());

    HWND parent = owner != nullptr ? GetAncestor (static_cast<HWND> (owner), GA_ROOT) : GetActiveWindow();
    if (dialog->Show (parent) != S_OK)
        return std::nullopt;
    Com<IShellItem> item;
    if (FAILED (dialog->GetResult (item.put())))
        return std::nullopt;
    PWSTR path = nullptr;
    if (FAILED (item->GetDisplayName (SIGDN_FILESYSPATH, &path)) || path == nullptr)
        return std::nullopt;
    std::filesystem::path result (path);
    CoTaskMemFree (path);
    return result;
}
} // namespace
#endif

std::optional<std::filesystem::path> chooseFile (const std::string& title, const std::vector<FileFilter>& filters, void* owner)
{
#ifdef _WIN32
    return showDialog (false, title, filters, owner);
#else
    (void) title;
    (void) filters;
    (void) owner;
    return std::nullopt;
#endif
}

std::optional<std::filesystem::path> chooseFolder (const std::string& title, void* owner)
{
#ifdef _WIN32
    return showDialog (true, title, {}, owner);
#else
    (void) title;
    (void) owner;
    return std::nullopt;
#endif
}

void reveal (const std::filesystem::path& path)
{
#ifdef _WIN32
    std::error_code ec;
    if (std::filesystem::is_directory (path, ec))
        ShellExecuteW (nullptr, L"open", path.wstring().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    else
    {
        auto args = L"/select,\"" + path.wstring() + L"\"";
        ShellExecuteW (nullptr, L"open", L"explorer.exe", args.c_str(), nullptr, SW_SHOWNORMAL);
    }
#else
    (void) path;
#endif
}

void openExternally (const std::string& target)
{
#ifdef _WIN32
    auto wide = paths::fromUtf8 (target).wstring();
    ShellExecuteW (nullptr, L"open", wide.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#else
    (void) target;
#endif
}

} // namespace plenio::platform
