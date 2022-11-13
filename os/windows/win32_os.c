#include "../os.h"
#include "../os_memory.h"

global OS_App* g_application;
global b32* running;



internal OS_Win32Handle*
OS_GetWin32Handle(OS_App* app)
{
    return (OS_Win32Handle*)app->handle;
}



torn_function void*
OS_Allocate(i32 size)
{
    TORN_Log("OS: Win32: Virtual Allocated %i\n", size);
    void* allocated_memory; 
    allocated_memory = VirtualAlloc(0, size,  MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    return allocated_memory;
}

torn_function void
OS_Free(void* address, u32 size)
{
    TORN_Log("OS: Win32: Freed at %p and the size freed was %i\n", address, size);
    VirtualFree(address, size, MEM_DECOMMIT | MEM_RELEASE);
}

LRESULT CALLBACK 
W32_WindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT ret = 0;
    
    switch (message)
    {
        
        case (WM_LBUTTONDOWN):
        {
            if (g_application != 0)
                g_application->event.type = OS_EventLMousePressed;
        } break;
        
        
        case (WM_RBUTTONDOWN):
        {
            if (g_application != 0)
                g_application->event.type = OS_EventRMousePressed;
        } break;
        
        case (WM_KEYDOWN):
        {
            if (g_application->key_pressed != 0)
                g_application->key_pressed(wparam);
#if 1  // TODO: Remove this, and add a proper queue system!
            if (wparam == VK_ESCAPE)
                *running = 0;
#endif 
        } break;
        
        
        case (WM_KEYUP):
        {
            if (g_application->key_released != 0)
                g_application->key_released(wparam);
            
        } break;
        case(WM_SIZE):
        {
            if (g_application != 0){
                RECT client_rectangle = {0};
                GetClientRect(window, &client_rectangle);
                r32 width  = client_rectangle.right - client_rectangle.left;
                r32 height = client_rectangle.bottom - client_rectangle.top;
                
                RECT window_rectangle;
                GetWindowRect(window, &window_rectangle);
                g_application->pos.x = window_rectangle.left;
                g_application->pos.y = window_rectangle.top;
                g_application->size.w = width;
                g_application->size.h = height;
                TORN_Log("OS: Win32: Window was resized:\n- x: %i\n- y: %i\n- w: %i\n- h: %i\n", g_application->pos.x, g_application->pos.y, g_application->size.w, g_application->size.h);
                if (g_application->style & OS_AppStyleViewportResize)
                {
                    
                    GFX_Resize(v2i(0,0), g_application->size);
                    TORN_Log("OS: WIN32: Changed viewport (x: 0,y: 0), (w: %i, h: %i)\n", g_application->size.w, g_application->size.h);
                    TORN_Log("OS: WIN32: This resize occured because OS_AppStyleViewportResize was toggled on.\n");
                }
                
            }
        } break;
        
        case (WM_QUIT):
        case (WM_DESTROY):
        {
            exit(0);
            PostQuitMessage(0); 
            *running = 0;
        }
        break;
        default:
        {
            
            
            if (g_application != 0) {
                g_application->event.type = OS_EventNothing;
                g_application->event.key = 0;
            }
            ret = DefWindowProc(window, message, wparam, lparam);
        } break;
    };
    
    return ret;
}

torn_function OS_App*
OS_ConstructApp(const u8* name, V2F size, V2F pos, OS_AppStyle style)
{
    
    OS_App* application = (OS_App*)MEM_ArenaAlloc(MEM_GetDefaultArena(), sizeof(OS_App));
    running = &application->running;
    application->name = name;
    application->style = style;
    application->handle = (OS_Win32Handle*)MEM_ArenaAlloc(MEM_GetDefaultArena(), sizeof(OS_Win32Handle));
    
    WNDCLASSEX window_class = {0};
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.lpszClassName = "TORN_WINDOW_CLASS";
    window_class.lpfnWndProc = W32_WindowProc;
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.hCursor = LoadCursor(0, IDC_ARROW);
    window_class.hInstance = GetModuleHandle(0);
    
    if (RegisterClassEx(&window_class))
    {
        TORN_Log("OS: Win32: Registering Window Class\n");
        OS_Win32Handle* handle = OS_GetWin32Handle(application);
        
        u32 flags = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;
        
        if (style & OS_AppStyleVisible)
            flags |= WS_VISIBLE;
        if (style & OS_AppStyleMinimize)
            flags |= WS_MINIMIZEBOX;
        if (style & OS_AppStyleMaximize)
            flags |= WS_MAXIMIZEBOX;
        
        handle->window = CreateWindowEx(WS_EX_APPWINDOW ,
                                        window_class.lpszClassName, 
                                        name,
                                        flags,
                                        pos.x,
                                        pos.y,
                                        size.w,
                                        size.h,
                                        0,
                                        0,
                                        GetModuleHandle(0),
                                        0);
        
        if (handle->window == 0)
            return 0;
        
        
        RECT client_rectangle = {0};
        GetClientRect(handle->window, &client_rectangle);
        r32 width  = client_rectangle.right - client_rectangle.left;
        r32 height = client_rectangle.bottom - client_rectangle.top;
        
        RECT window_rectangle;
        GetWindowRect(handle->window, &window_rectangle);
        application->pos.x = window_rectangle.left;
        application->pos.y = window_rectangle.top;
        application->size.w = width;
        application->size.h = height;
        TORN_Log("OS: Win32: Created Window:\n- name: %s\n- x: %i\n- y: %i\n- w: %i\n- h: %i\n",
                 name, application->pos.x, application->pos.y, application->size.w,application->size.h);
        *running = 1;
        g_application = application;
        TORN_Log("OS: Win32: Constructed application successfully!\n");
        
        LARGE_INTEGER freq;
        LARGE_INTEGER time;
        
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&time);
        
        handle->time_start = (1000 * time.QuadPart) / freq.QuadPart;
        
        
        return application;
    }
    return 0;
}

torn_function void 
OS_PollEvents(OS_App* app)
{
    MSG message;
    if (PeekMessage(&message, 0,0,0, PM_REMOVE)){
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
    
    
    
}
torn_function void 
OS_WaitEvents(OS_App* app)
{
    MSG message;
    if (GetMessage(&message, 0,0,0) > 0){
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}


torn_function V2F
OS_GetMousePosition(OS_App* app)
{
    POINT point;
    GetCursorPos(&point);
    ScreenToClient( OS_GetWin32Handle(app)->window, &point);
    return v2(point.x, point.y);
}

torn_function void
OS_MessageBox(OS_App* app, const u8* contents, const u8* titlebar, OS_MessageBoxFlags flags)
{
    
    HWND window = app == 0 ? 0 : OS_GetWin32Handle(app)->window;
    TORN_Log("OS: Win32: MessageBox(%s, %s)\n", contents, titlebar);
    switch (flags)
    {
        case (OS_MessageBox_Error):
        {
            MessageBox(window, contents, titlebar, MB_ICONERROR | MB_OK);
        } break;
        
        case (OS_MessageBox_Warning):
        {
            MessageBox(window, contents, titlebar, MB_ICONWARNING | MB_OK);
        } break;
        
        case (OS_MessageBox_Info):
        {
            MessageBox(window, contents, titlebar, MB_ICONINFORMATION | MB_OK);
        } break;
        default:
        {
            MessageBox(window, contents, titlebar, MB_OK);
        } break;
    }
    
}
torn_function void
OS_SetWindowTitle(OS_App* app, const char* title)
{
    TORN_Log("OS: Win32: Window title was changed to \"%s\"\n", title);
    app->name = title;
    SetWindowTextA(
                   OS_GetWin32Handle(app)->window,
                   title
                   );
}

torn_function V2F
OS_GetPrimaryMonitorSize(void)
{
    return v2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}
torn_function void
OS_SetWindowBorderedFullscreen(OS_App* app)
{
    TORN_Log("OS: Win32: Bordered Fullscreen enabled\n");
    
    V2F monitor_size = OS_GetPrimaryMonitorSize();
    
    SetWindowPos(OS_GetWin32Handle(app)->window,
                 HWND_TOPMOST,
                 0,
                 0,
                 monitor_size.w,
                 monitor_size.h,
                 SWP_SHOWWINDOW);
    
}
torn_function void
OS_SetWindowBorderlessFullscreen(OS_App* app)
{
    TORN_Log("OS: Win32: Borderless Fullscreen enabled\n");
    OS_SetWindowBorderedFullscreen(app);
    HWND window = OS_GetWin32Handle(app)->window;
    SetWindowLong(window,
                  GWL_STYLE,
                  0);
    ShowWindow(window, SW_SHOW);
}

torn_function r32
OS_GetTime(OS_App* app)
{
    
    LARGE_INTEGER freq;
    LARGE_INTEGER time;
    
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&time);
    OS_Win32Handle* handle = OS_GetWin32Handle(app);
    
    return ((( 1000* time.QuadPart) / freq.QuadPart) - handle->time_start)/1000;
    
}

torn_function u8* 
OS_ReadFile(const u8* path)
{
    HANDLE file = CreateFile(path,
                             GENERIC_READ,
                             FILE_SHARE_READ,
                             0,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             0);
    if (file == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    LARGE_INTEGER size;
    GetFileSizeEx(file, 
                  &size);
    i32 bytes = size.QuadPart;
    TORN_Log("OS: Win32: Attempting to read file with %i bytes\n", bytes);
    char* read_buffer = MEM_ArenaAlloc(MEM_GetDefaultArena(), bytes);
    DWORD bytes_read = 0;
    if (ReadFile(file, read_buffer, bytes,  &bytes_read, 0))
    {
        TORN_Log("OS: Win32: Finished reading %s!\n", path);
        read_buffer[bytes_read+1] = '\0';
        return read_buffer;
    }
    return 0;
    
}

torn_function u8
OS_SaveFile(const u8* path, const u8* contents)
{
    HANDLE file = CreateFile(path,
                             GENERIC_WRITE,
                             FILE_SHARE_READ,
                             0,
                             CREATE_NEW,
                             FILE_ATTRIBUTE_NORMAL,
                             0);
    if (file == INVALID_HANDLE_VALUE)
    {
        return 0;
    }
    
    
    TORN_Log("OS: Win32: Attempting to write file");
    
    i32 bytes = 0;
    while (contents[bytes++] != '\0');
    i32 byte_written = 0;
    
    
    if (WriteFile(file, contents, bytes-1,  &byte_written, 0))
    {
        TORN_Log("OS: Win32: Finished writing to %s! (bytes %i)\n", path, byte_written);
        return 1;
    }
    return 0;
    
}

torn_function OS_App*
OS_GetActiveApp(void)
{
    return g_application;
}


// Events



torn_function OS_KeyState 
OS_GetKeyState(i8 key)
{
    OS_KeyState state;
    SHORT vk_state = GetAsyncKeyState(key );
    state.pressed = ((1 << 15) & vk_state);
    state.released = !state.pressed;
    return state;
}



torn_function OS_MouseState
OS_GetLeftMouseState(void)
{
    OS_MouseState state = {0};
    OS_KeyState key_state = OS_GetKeyState(VK_LBUTTON);
    state.clicked = key_state.pressed;
    state.released = key_state.released;
    return state;
    
}


torn_function OS_MouseState
OS_GetRightMouseState(void)
{
    OS_MouseState state = {0};
    OS_KeyState key_state = OS_GetKeyState(VK_RBUTTON);
    state.clicked = key_state.pressed;
    state.released = key_state.released;
    return state;
    
}


int WINAPI 
WinMain(HINSTANCE instance,
        HINSTANCE previous_instance,
        LPSTR command_line,
        int cmd_show)
{
    TORN_Log("OS: Win32: WinMain entry point\n");
    return main(__argc, __argv);
}



