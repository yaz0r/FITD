# FITD

Free in the Dark, a Alone in the Dark engine reimplementation.

Long overdue source code update from the version that was released on source forge in the 2000s.
Only buildable with Visual Studio 2022 for now but other platform should be fairly straight forward.

Rendering is being rewritten to bgfx, for better portability. There is still some graphical artifacts.

## Build instructions
### Visual Studio - Recommended
* Requires VS2022 with CMake module installed (you can also provide your own cmake but vs2022.bat won't work out of the box).
* Clone the repo with recursive submodules enabled (`git clone --recurse-submodules -j8 https://github.com/yaz0r/FITD.git`)
* Run build/vs2022.bat this should detect vs2022/cmake and generate the solution
* Open solution build/vs2022/FITD.sln
* Set fitd as startup project
* Set fitd working directory to where the game files live
* Hit F5
### Visual Studio Code
Currently builds on Linux but errors on running. Untested on Windows & macOs, but will almost certainly build if either using the Dev Container or having the needed dependancies installed locally.
#### Dev Container - requires Docker to be installed locally
1. Clone the repo with recursive submodules enabled (`git clone --recurse-submodules -j8 https://github.com/yaz0r/FITD.git`)
2. Open Visual Studio Code
3. Add the repo to the workspace
4. Select `Reopen in Container` on the pop-up in the bottom right, or click the green arrows in the bottom left and select `Reopen in Container` in the menu.
5. Once the container is set up, select `Scan kits` in the menu that appears.
6. Use the CMake menu to configure all projects
7. Click the Build icon on CMake > Project Outline > FITD > Fitd to build
#### Local - requires dependancies to be installed locally
1. Clone the repo with recursive submodules enabled (`git clone --recurse-submodules -j8 https://github.com/yaz0r/FITD.git`)
2. Open Visual Studio Code
4. Install the CMake extension
3. Add the repo to the workspace
5. Select `Scan kits` in the menu that appears.
6. Use the CMake menu to configure all projects
7. Click the Build icon on CMake > Project Outline > FITD > Fitd to build

## Screenshots

![image](https://github.com/user-attachments/assets/d12b7c66-6c57-4507-b2b3-540cb2cc6806)
![image](https://github.com/user-attachments/assets/ba4508ba-1b7f-4ac1-b573-169e3f8e7bab)
![image](https://github.com/user-attachments/assets/653d3dc5-4a35-4a7f-95bc-76e7b42899d9)
![image](https://github.com/user-attachments/assets/310c3151-18cf-4914-8992-7de1dd763653)

