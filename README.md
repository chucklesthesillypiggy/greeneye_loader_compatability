# Greeneye Loader Compatibility Handoff

A small Windows x64 `dbghelp.dll` that hands the dedicated server's dump-function imports to an existing **Greeneye Loader `dbgcore.dll`**.

This was developed for an Enshrouded dedicated server hosted where loading Greeneye through its other DLL filenames was unsuccessful. The user subsequently confirmed that the handoff loaded Greeneye and its server mods. It may help installations with the same import/loading path; it is not a universal fix for hosted servers.

## How it works

Windows resolves these PE export forwarders through its normal DLL import mechanism:

| Handoff export | Target |
| --- | --- |
| `MiniDumpWriteDump` | `dbgcore.MiniDumpWriteDump` |
| `MiniDumpReadDumpStream` | `dbgcore.MiniDumpReadDumpStream` |

The handoff uses no worker thread, process injection or game-executable patch. Its own code imports only `KERNEL32.dll`. Greeneye performs the actual mod loading. Greeneye Loader itself and game binaries are **not included** in this repository.

## Build

Install Visual Studio 2022 Build Tools with the C++ tools and Windows SDK. Open an **x64 Native Tools Command Prompt**, change to this repository and run:

```bat
build.bat
```

The output is `build\dbghelp.dll`. The build uses the Windows SDK and no C runtime library. Build as x64; the supplied server and loader are x64.

## Usage

1. Upload the built `dbghelp.dll` and a compatible, separately obtained Greeneye Loader named `dbgcore.dll` beside `enshrouded_server.exe` or `enshrouded.exe`.
2. Keep your existing `greeneye/` mods and configuration in place.
3. Start the server from the panel.

Expected layout:

```text
enshrouded_server.exe | enshrouded.exe
dbghelp.dll
dbgcore.dll
greeneye/
    <installed mods>
```

The handoff does not chain an existing loader that occupies `dbghelp.dll`. Do not replace unrelated DLLs. To uninstall, stop the server, remove this handoff and restore your backed-up `dbghelp.dll` if applicable.

## Diagnostics

The handoff appends `greeneye-handoff.log` beside itself. It records the executable path, its own path and the visible `dbgcore` module at process attach. Greeneye writes its own `greeneye/loader.log`.

If no logs appear, check the actual running directory, write permissions and the host's DLL loading policy.

## Scope

This is a minimal two-export handoff, **not a complete implementation of Microsoft's DbgHelp API**. Other executables or future builds that import additional DbgHelp functions need further review. A compatible Greeneye `dbgcore.dll` must provide the forwarded exports. The handoff cannot override a hosting provider's DLL loading restrictions.

## Credits and license

Greeneye Loader is a separate project by Greeneye0; this repository contains the compatibility handoff, not the loader's source or binary. The handoff source is available under the [MIT license](LICENSE).
