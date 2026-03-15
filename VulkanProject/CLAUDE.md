# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

Open `VulkanProject.sln` in Visual Studio 2022 and build with **Ctrl+Shift+B**, or use MSBuild from the command line:

```bat
msbuild VulkanProject.sln /p:Configuration=Debug /p:Platform=x64
```

The output executable is at `x64\Debug\VulkanProject.exe`.

## External Dependencies

Paths are hardcoded in the `.vcxproj` — do not move these:

| Library | Path |
|---------|------|
| Vulkan SDK 1.4.313.0 | `C:\VulkanSDK\1.4.313.0\` |
| GLFW (include) | `D:\DXStudy\externals\glfw\include` |
| GLFW (lib) | `D:\DXStudy\externals\glfw\lib-vc2022` |
| GLM | `D:\DXStudy\externals\glm` |

## Architecture

The app follows a simple init → loop → cleanup pattern:

- **`Main.cpp`** — entry point. Creates a GLFW window (800×600, non-resizable, no OpenGL), instantiates `VulkanRenderer`, runs the poll loop, then calls `CleanUp()`.
- **`VulkanRenderer`** — owns all Vulkan state. `init()` calls these in order:
  1. `CreateInstance()` — gathers GLFW-required extensions, creates `VkInstance` (Vulkan API 1.4, no validation layers).
  2. `CreateSurface()` — creates a Win32 surface (`VkSurfaceKHR`) via `vkCreateWin32SurfaceKHR`.
  3. `GetPhysicalDevice()` — picks the first GPU that passes `CheckDeviceSuitable()`.
  4. `CreateLogicalDevice()` — creates `VkDevice` with a single graphics queue.
- **`Utilities.h`** — defines `QueueFamilyIndices` (holds `GraphicsFamily` index, `IsValid()` returns `GraphicsFamily >= 0`).

## Platform Notes

- Windows-only: uses `VK_USE_PLATFORM_WIN32_KHR` and `GLFW_EXPOSE_NATIVE_WIN32`.
- Validation layers are currently disabled (`enabledLayerCount = 0`). To enable them, add `VK_LAYER_KHRONOS_validation` to `CreateInstance()`.
- The x64 Debug config links against `C:\VulkanSDK\1.4.313.0\Lib` (64-bit); Win32 configs use `Lib32`.
