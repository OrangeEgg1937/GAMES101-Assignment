## Debug with CMake

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug Transformation",
      "type": "cppdbg", // Use "lldb" on macOS, "cppvsdbg" for Windows MSVC
      "request": "launch",
      "program": "${workspaceFolder}/build/Transformation", // Executable path
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}/build",
      "environment": [],
      "externalConsole": false,
      "MIMode": "lldb", // Use "lldb" on macOS
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ]
    }
  ]
}
```