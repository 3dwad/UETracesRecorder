#include <windows.h>
#include <shobjidl.h>

#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr wchar_t kWindowClassName[] = L"UETracesRecorderWindowClass";
constexpr wchar_t kWindowTitle[] = L"UE Traces Recorder";
constexpr wchar_t kSettingsFileName[] = L"UETracesRecorder.settings";
constexpr wchar_t kSingleInstanceMutexName[] = L"UETracesRecorder.SingleInstanceMutex";

constexpr int kControlStartX = 16;
constexpr int kControlStartY = 16;
constexpr int kWindowWidth = 760;
constexpr int kWindowHeight = 520;

constexpr int kExePathEditId = 1001;
constexpr int kBrowseButtonId = 1002;
constexpr int kLaunchButtonId = 1003;
constexpr int kStatusLabelId = 1004;

struct TraceChannel {
    const wchar_t* display_name;
    const wchar_t* arg_name;
    int control_id;
    bool checked_by_default;
};

constexpr std::array<TraceChannel, 25> kChannels = {{
    {L"CPU", L"cpu", 2001, true},
    {L"GPU", L"gpu", 2002, true},
    {L"Frame", L"frame", 2003, true},
    {L"Log", L"log", 2004, true},
    {L"Bookmark", L"bookmark", 2005, true},
    {L"Screenshot", L"screenshot", 2006, true},
    {L"Task", L"task", 2007, false},
    {L"TaskGraph", L"taskgraph", 2008, false},
    {L"Counter", L"counter", 2009, false},
    {L"Stats", L"stats", 2010, false},
    {L"Memory", L"memory", 2011, false},
    {L"Module", L"module", 2012, false},
    {L"Metadata", L"metadata", 2013, false},
    {L"LoadTime", L"loadtime", 2014, false},
    {L"AssetLoadTime", L"assetloadtime", 2015, false},
    {L"Cook", L"cook", 2016, false},
    {L"File", L"file", 2017, false},
    {L"Net", L"net", 2018, false},
    {L"Object", L"object", 2019, false},
    {L"Animation", L"animation", 2020, false},
    {L"Audio", L"audio", 2021, false},
    {L"Niagara", L"niagara", 2022, false},
    {L"RenderCommands", L"rendercommands", 2023, false},
    {L"RHICommands", L"rhicommands", 2024, false},
    {L"RDG", L"rdg", 2025, false},
}};

std::wstring GetExeDirectory() {
    wchar_t path[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
    if (length == 0 || length == MAX_PATH) {
        return L".";
    }
    std::filesystem::path exe_path(path);
    return exe_path.parent_path().wstring();
}

std::wstring GetSettingsPath() {
    return (std::filesystem::path(GetExeDirectory()) / kSettingsFileName).wstring();
}

void SetStatus(HWND window, const std::wstring& text) {
    SetWindowTextW(GetDlgItem(window, kStatusLabelId), text.c_str());
}

std::wstring BuildTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto time_t_value = std::chrono::system_clock::to_time_t(now);
    std::tm tm_value = {};
    localtime_s(&tm_value, &time_t_value);

    wchar_t buffer[64] = {};
    wcsftime(buffer, sizeof(buffer) / sizeof(wchar_t), L"%Y-%m-%d_%H-%M-%S", &tm_value);
    return buffer;
}

std::wstring ReadSavedExePath() {
    std::wifstream input(GetSettingsPath());
    if (!input.is_open()) {
        return L"";
    }
    std::wstring path;
    std::getline(input, path);
    return path;
}

void SaveExePath(const std::wstring& exe_path) {
    std::wofstream output(GetSettingsPath(), std::ios::trunc);
    if (output.is_open()) {
        output << exe_path;
    }
}

std::wstring GetEditText(HWND edit_control) {
    const int length = GetWindowTextLengthW(edit_control);
    if (length <= 0) {
        return L"";
    }
    std::vector<wchar_t> buffer(static_cast<size_t>(length) + 1, L'\0');
    GetWindowTextW(edit_control, buffer.data(), length + 1);
    return std::wstring(buffer.data());
}

bool IsChecked(HWND window, int control_id) {
    return SendMessageW(GetDlgItem(window, control_id), BM_GETCHECK, 0, 0) == BST_CHECKED;
}

std::wstring BuildTraceChannelsArg(HWND window) {
    std::vector<std::wstring> selected;
    for (const auto& channel : kChannels) {
        if (IsChecked(window, channel.control_id)) {
            selected.emplace_back(channel.arg_name);
        }
    }

    std::wstringstream arg;
    for (size_t i = 0; i < selected.size(); ++i) {
        arg << selected[i];
        if (i + 1 < selected.size()) {
            arg << L",";
        }
    }
    return arg.str();
}

std::wstring QuoteIfNeeded(const std::wstring& value) {
    if (value.find(L' ') != std::wstring::npos) {
        return L"\"" + value + L"\"";
    }
    return value;
}

bool SelectPackedBuildExe(HWND owner, std::wstring& result_path) {
    IFileDialog* file_dialog = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&file_dialog));
    if (FAILED(hr) || file_dialog == nullptr) {
        return false;
    }

    DWORD options = 0;
    file_dialog->GetOptions(&options);
    file_dialog->SetOptions(options | FOS_FILEMUSTEXIST | FOS_PATHMUSTEXIST);

    COMDLG_FILTERSPEC filters[] = {
        {L"Windows executable (*.exe)", L"*.exe"},
        {L"All files (*.*)", L"*.*"},
    };
    file_dialog->SetFileTypes(2, filters);

    hr = file_dialog->Show(owner);
    if (SUCCEEDED(hr)) {
        IShellItem* item = nullptr;
        hr = file_dialog->GetResult(&item);
        if (SUCCEEDED(hr) && item != nullptr) {
            PWSTR selected = nullptr;
            hr = item->GetDisplayName(SIGDN_FILESYSPATH, &selected);
            if (SUCCEEDED(hr) && selected != nullptr) {
                result_path = selected;
                CoTaskMemFree(selected);
                item->Release();
                file_dialog->Release();
                return true;
            }
            if (selected != nullptr) {
                CoTaskMemFree(selected);
            }
            item->Release();
        }
    }

    file_dialog->Release();
    return false;
}

void LaunchProjectWithTrace(HWND window) {
    HWND edit = GetDlgItem(window, kExePathEditId);
    std::wstring exe_path = GetEditText(edit);
    if (exe_path.empty()) {
        SetStatus(window, L"Please select a packaged .exe first.");
        return;
    }

    if (!std::filesystem::exists(exe_path)) {
        SetStatus(window, L"Selected .exe does not exist.");
        return;
    }

    std::wstring trace_channels = BuildTraceChannelsArg(window);
    if (trace_channels.empty()) {
        SetStatus(window, L"Select at least one trace channel.");
        return;
    }

    const std::wstring trace_file = (std::filesystem::path(GetExeDirectory()) /
        (L"UETrace_" + BuildTimestamp() + L".utrace")).wstring();

    std::wstring command_line = QuoteIfNeeded(exe_path);
    command_line += L" -trace=" + trace_channels;
    command_line += L" -tracefile=" + QuoteIfNeeded(trace_file);
    command_line += L" -statnamedevents";

    STARTUPINFOW startup_info = {};
    startup_info.cb = sizeof(startup_info);
    PROCESS_INFORMATION process_info = {};

    std::wstring mutable_command_line = command_line;
    const BOOL launched = CreateProcessW(
        nullptr,
        mutable_command_line.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &startup_info,
        &process_info
    );

    if (!launched) {
        SetStatus(window, L"Failed to launch build. Check executable path.");
        return;
    }

    CloseHandle(process_info.hThread);
    CloseHandle(process_info.hProcess);

    SaveExePath(exe_path);
    SetStatus(window, L"Launch successful. Trace will be written near UETracesRecorder.exe");
}

void CreateUi(HWND window) {
    CreateWindowExW(0, L"STATIC", L"Packaged build .exe:", WS_CHILD | WS_VISIBLE,
        kControlStartX, kControlStartY, 180, 20, window, nullptr, nullptr, nullptr);

    CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
        kControlStartX, kControlStartY + 20, 560, 24, window,
        reinterpret_cast<HMENU>(kExePathEditId), nullptr, nullptr);

    CreateWindowExW(0, L"BUTTON", L"Browse...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        kControlStartX + 570, kControlStartY + 20, 150, 24, window,
        reinterpret_cast<HMENU>(kBrowseButtonId), nullptr, nullptr);

    CreateWindowExW(0, L"STATIC", L"Trace channels (6 default selected):", WS_CHILD | WS_VISIBLE,
        kControlStartX, kControlStartY + 56, 350, 20, window, nullptr, nullptr, nullptr);

    constexpr int kColumns = 3;
    constexpr int kRowHeight = 26;
    constexpr int kColumnWidth = 230;
    constexpr int kChannelsStartY = kControlStartY + 80;

    for (size_t i = 0; i < kChannels.size(); ++i) {
        const auto& channel = kChannels[i];
        const int row = static_cast<int>(i / kColumns);
        const int col = static_cast<int>(i % kColumns);
        const int x = kControlStartX + (col * kColumnWidth);
        const int y = kChannelsStartY + (row * kRowHeight);

        HWND checkbox = CreateWindowExW(0, L"BUTTON", channel.display_name,
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            x, y, 220, 20, window,
            reinterpret_cast<HMENU>(channel.control_id), nullptr, nullptr);
        SendMessageW(checkbox, BM_SETCHECK,
            channel.checked_by_default ? BST_CHECKED : BST_UNCHECKED, 0);
    }

    const int rows = (static_cast<int>(kChannels.size()) + kColumns - 1) / kColumns;
    const int launch_y = kChannelsStartY + (rows * kRowHeight) + 10;

    CreateWindowExW(0, L"BUTTON", L"Launch with trace", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
        kControlStartX, launch_y, 200, 32, window,
        reinterpret_cast<HMENU>(kLaunchButtonId), nullptr, nullptr);

    CreateWindowExW(0, L"STATIC", L"Status: waiting", WS_CHILD | WS_VISIBLE,
        kControlStartX, launch_y + 42, 700, 50, window,
        reinterpret_cast<HMENU>(kStatusLabelId), nullptr, nullptr);

    std::wstring saved_path = ReadSavedExePath();
    if (!saved_path.empty()) {
        SetWindowTextW(GetDlgItem(window, kExePathEditId), saved_path.c_str());
    }
}

LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
    switch (message) {
        case WM_CREATE:
            CreateUi(window);
            return 0;

        case WM_COMMAND: {
            const int control_id = LOWORD(w_param);
            if (control_id == kBrowseButtonId) {
                std::wstring path;
                if (SelectPackedBuildExe(window, path)) {
                    SetWindowTextW(GetDlgItem(window, kExePathEditId), path.c_str());
                    SetStatus(window, L"Executable selected.");
                }
                return 0;
            }
            if (control_id == kLaunchButtonId) {
                LaunchProjectWithTrace(window);
                return 0;
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProcW(window, message, w_param, l_param);
    }
}

}  // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show_command) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    HANDLE app_mutex = CreateMutexW(nullptr, FALSE, kSingleInstanceMutexName);
    if (app_mutex == nullptr) {
        CoUninitialize();
        return -1;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(nullptr, L"UE Traces Recorder is already running.", L"UE Traces Recorder", MB_OK | MB_ICONINFORMATION);
        CloseHandle(app_mutex);
        CoUninitialize();
        return 0;
    }

    WNDCLASSW window_class = {};
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = kWindowClassName;
    window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&window_class)) {
        CloseHandle(app_mutex);
        CoUninitialize();
        return -1;
    }

    HWND window = CreateWindowExW(
        0,
        kWindowClassName,
        kWindowTitle,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        kWindowWidth,
        kWindowHeight,
        nullptr,
        nullptr,
        instance,
        nullptr
    );

    if (window == nullptr) {
        CloseHandle(app_mutex);
        CoUninitialize();
        return -1;
    }

    ShowWindow(window, show_command);

    MSG message = {};
    while (GetMessageW(&message, nullptr, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    CloseHandle(app_mutex);
    CoUninitialize();
    return 0;
}
