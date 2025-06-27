workspace "3D-Engine"
    configurations { "Debug", "Release" }

project "Engine"
    kind "StaticLib"
    location "engine"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    optimize "On"
    
    files { "engine/**.h", "engine/**.c" }
    includedirs { "engine/vendor/glfw/include", "engine/vendor", "engine/inc" }

    filter "system:windows"
        defines { "_GLFW_WIN32" }
        links { "gdi32" }

    filter "system:linux"
        defines { "_GLFW_X11" }
        links { "GL", "m" }

project "Sandbox"
    kind "ConsoleApp"
    location "sandbox"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    optimize "On"

    files { "sandbox/**.c" }
    includedirs { "engine/vendor", "engine/inc" }
    links { "Engine" }

    filter "system:windows"
        defines { "_GLFW_WIN32" }
        links { "gdi32" }

    filter "system:linux"
        defines { "_GLFW_X11" }
        links { "GL", "m" }