# FITD

Free in the Dark, a Alone in the Dark engine reimplementation.

Long overdue source code update from the version that was released on source forge in the 2000s.
Only buildable with Visual Studio 2022 for now but other platform should be fairly straight forward.

Rendering is being rewritten to bgfx, for better portability. There is still some graphical artifacts.
#### Table of Contents
* [Screenshots](#screenshots)
* [Build instructions](#build-instructions)
  * [Visual Studio (Recommended & Validated)](#visual-studio-windows-recommended--validated)
  * [Visual Studio Code](#visual-studio-code)
    * [Dev Container (Any Platform)](#dev-container-any-platform)
    * [Local (Linux, untested for macOS)](#local-linux-untested-for-macos)
* [Platform Notes](#platform-notes)
  * [Building with Visual Studio on Windows](building-with-visual-studio-on-windows)
  * [Building with Vs Code in the Dev Container](building-with-vs-code-in-the-dev-container)

## Screenshots
![image](https://github.com/user-attachments/assets/d12b7c66-6c57-4507-b2b3-540cb2cc6806)
![image](https://github.com/user-attachments/assets/ba4508ba-1b7f-4ac1-b573-169e3f8e7bab)
![image](https://github.com/user-attachments/assets/653d3dc5-4a35-4a7f-95bc-76e7b42899d9)
![image](https://github.com/user-attachments/assets/310c3151-18cf-4914-8992-7de1dd763653)

## Build instructions
### Visual Studio (Windows, Recommended & Validated)
This requires Git & VS2022 with CMake module installed (you can also provide your own cmake but vs2022.bat won't work out of the box).
1. Clone the repo with recursive submodules enabled (`git clone --recurse-submodules -j8 https://github.com/yaz0r/FITD.git`)
1. Run `build/vs2022.bat`; this should detect vs2022/cmake and generate the solution
1. Open generated solution `build/vs2022/FITD.sln` in Visual Studio
1. Set fitd as the startup project
1. Set fitd working directory to where the game files live (recommended to be in a `data` folder at the root of the repository; this is already added to .gitignore)
1. Hit F5 to build & run
### Visual Studio Code
Currently builds on Linux both in the Dev Container & locally, but [has fatal pre-gameplay runtime errors](https://github.com/yaz0r/FITD/issues/9). Untested on Windows & macOS, but will almost certainly build if using the Dev Container; might build locally for macOS if the needed dependancies are installed locally. However, actually running the executable using the build tools from inside the Dev Container will almost certainly fail when made on Windows & very likely fail on macOS (though ***maybe*** not if the proper dependancies are installed locally). That being said, this is technically untested & ***might*** work if the configuration is drastically changed; feel free to test this. For all builds made inside the Dev Container on any platform, carefully read the [Platform Notes](#platform-notes) section.
#### Dev Container (Any Platform)
This requires Docker & Git to be installed locally. If on Windows w/ WSL, Git & Docker must be installed natively; you *do not* need to install them through WSL.
1. Clone the repo with recursive submodules enabled (`git clone --recurse-submodules -j8 https://github.com/yaz0r/FITD.git`)
2. Open the repository in Visual Studio Code
3. Select `Reopen in Container` on the notification pop-up in the bottom right, or click the green arrows in the bottom left and select `Reopen in Container` in the menu.
4. Once the container is set up, the extensions will install. As CMake installs, a menu will appear asking what compilers to use. Select `Scan kits`.
  * If this menu appears again, select the GCC option.
5. Use the CMake menu to configure all projects
6. Click the Build icon on `CMake > Project Outline > FITD > Fitd` to build
7. Place the game files (e.g. for a Steam install of AITD1, the contents of the `INDARK` folder) in a specified directory (recommended to be in a `data` folder at the root of the repository; this is already added to .gitignore, & both the `launcher` script & the `Run & Debug` tools can be used to properly launch if this location is used)
8. To run the build, either manually execute the file from outside of the Dev Container (users with Bash installed locally can use the script) or exit the Dev Container and use Vs Code's `Run & Debug` tools (see [Platform Notes](#platform-notes) for platform-specific details).
  * Be sure to change the working directory to the location of the game data before running the executable
    * If the game data is in a `data` folder at the root of the repository's directory, then both the `launcher` script & the `Run & Debug` tools can properly launch the executable, as they both change the working directory to this folder before launching the executable.
  * If not using the script & not using Vs Code's `Run & Debug` tools, placing the game files directly in the executable's directory & launching *directly from that directory* will also work.
#### Local (Linux, untested for macOS)
Requires dependencies to be installed locally; look at the end of the Dockerfile to see what those dependencies are.
1. Clone the repo with recursive submodules enabled (`git clone --recurse-submodules -j8 https://github.com/yaz0r/FITD.git`)
2. Open the repository in Visual Studio Code
3. Install the CMake extension if not already installed.
4. Select `Scan kits` in the menu that appears.
5. Use the CMake menu to configure all projects
6. Click the Build icon on CMake > Project Outline > FITD > Fitd to build
7. For Linux users

## Platform Notes
### Building with Visual Studio on Windows
This has been tested rigorously & works fine. Building for other plaforms from a Windows environment hasn't been robustly tested. If you'd like to try and build for Linux, please do so & contact @jmortiger ; he can test your build. If anyone has a macOS machine (Intel or Apple Silicon/M#) & a Windows machine that would like to test a macOS build, please do so and let us know how it goes. It would be great to make a proper release w/ a compiled binary for the 3 main platforms.
### Building with Vs Code in the Dev Container
Currently, the build ***cannot*** be run *properly* inside the Dev Container; you'll get a whole mess of device & driver errors. Feel free to play with the config, but don't get your hopes up. Additionally, currently, the build will *almost certainly* target Linux as the platform.
* If you're on Linux & just using the Dev Container for consistency with other contributors, you're all clear to run the executable outside of the Dev Container, either using a CLI, or through the `Run & Debug` tools. It hasn't been tested if the build will work without the required dependancies (like X11's developer files or X11 itself) being installed locally. If it does for you, let us know what packages listed in the Dockerfile weren't installed on your system & what you targeted (e.g Release or Debug) so we can update this readme, & maybe even add checks for that to our workflow. If you do have all the listed packages installed, it should run correctly (though there are still [bugs](https://github.com/yaz0r/FITD/issues/9) that all builds for Linux seem to face).
* If you're a Windows user, the build will have targeted Linux, so don't bother unless you're using WSL
  * If you are using WSL, please let us know how it goes; just read the prior bullet point for Linux users and replace "locally" & "on Linux" with "using WSL" to catch any simple gotchas. You might still run into the afformentioned errors of running in a Dev Container.
  * If you want to try playing with the configuration to force the Linux Dev Container to build for Windows without relying on WSL and get it to any level of working  beyond building to <25% & failing (e.g. it builds to more than 25% before failing, it almost completely builds, it completely builds but there's early fatal runtime errors, you can start playing but it eventually glitches and/or crashes, it works exactly the same as a native local Windows build made using Visual Studio, it works perfectly), please let us know; we'd love to update this readme & repo to make it easier for potential contributors to get up and running!
* If you're a macOS user, the build will have targeted Linux. It hasn't been tested on macOS, & it's highly unlikely to work, especially if you don't have the required dependancies (like X11's developer files or X11 itself) installed locally. That being said, there's little harm in trying; macOS, like Linux, is a Unix derivative, so you might get it to work; if you do, let us know! However, if you want to try playing with the configuration to get the Linux Dev Container to build for macOS and you can get it to any level of working beyond building to <25% & failing (e.g. it builds to more than 25% before failing, it almost completely builds, it completely builds but there's early fatal runtime errors, you can start playing but it eventually glitches/crashes, it works perfectly), please let us know; we'd love to update this readme & repo to make it easier for potential contributors to get up and running!
For all of the above cases, if run locally through VS Code's `Run & Debug` tools, the current debugger config will not correctly find locate the files; this is because the Dev Container has a different arrangement of directories than when outside of it. Most features will still work correctly (including call stack display & readout of local var's when stopped), but the directories are baked into the build output. To fix this, rebuild from outside the Dev Container (if possible).
