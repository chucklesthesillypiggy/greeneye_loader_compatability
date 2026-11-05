#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// No CRT, worker thread, process injection or executable patching. The PE export
// forwarders load dbgcore through Windows' normal import resolution.
static void Append(HANDLE file,const char* text) {
    DWORD bytes=0,written=0;const volatile char* cursor=text;while(cursor[bytes])++bytes;
    if(file!=INVALID_HANDLE_VALUE)WriteFile(file,text,bytes,&written,NULL);
}
static __declspec(noinline) void AppendPath(HANDLE file,const WCHAR* path) {
    char utf8[3072];int count=WideCharToMultiByte(CP_UTF8,0,path,-1,utf8,sizeof utf8,NULL,NULL);
    if(count>0){DWORD written;WriteFile(file,utf8,(DWORD)count-1,&written,NULL);}
    else Append(file,"(path unavailable)");
    Append(file,"\r\n");
}
static __declspec(noinline) void ReportModule(HANDLE file,HMODULE module,const char* title) {
    WCHAR path[1024];DWORD count=GetModuleFileNameW(module,path,1024);
    Append(file,title);
    if(count&&count<1024)AppendPath(file,path);
    else Append(file,"(path unavailable or too long)\r\n");
}
BOOL WINAPI DllMain(HINSTANCE module,DWORD reason,LPVOID reserved) {
    (void)reserved;
    if(reason!=DLL_PROCESS_ATTACH)return TRUE;
    DisableThreadLibraryCalls(module);
    WCHAR path[1024];DWORD count=GetModuleFileNameW(module,path,1024);
    if(!count||count>=1024)return TRUE;
    DWORD tail=count;while(tail&&path[tail-1]!=L'\\'&&path[tail-1]!=L'/')--tail;
    static const WCHAR name[]=L"greeneye-handoff.log";
    DWORD i=0;do{if(tail+i>=1024)return TRUE;path[tail+i]=name[i];}while(name[i++]);
    HANDLE file=CreateFileW(path,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if(file==INVALID_HANDLE_VALUE)return TRUE;
    Append(file,"\r\nGreeneye dbghelp handoff 1.0.0: DLL_PROCESS_ATTACH\r\n");
    ReportModule(file,NULL,"Executable: ");
    ReportModule(file,module,"Handoff DLL: ");
    HMODULE target=GetModuleHandleW(L"dbgcore.dll");
    if(target)ReportModule(file,target,"dbgcore module visible at attach: ");
    else Append(file,"dbgcore module not visible at this attach point; inspect Greeneye's loader.log\r\n");
    Append(file,"Exports forward to dbgcore.MiniDumpWriteDump and dbgcore.MiniDumpReadDumpStream\r\n");
    FlushFileBuffers(file);CloseHandle(file);return TRUE;
}
