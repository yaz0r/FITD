This lists the explicit CLI commands run by both the Github Actions workflow & the CMake plugin in the Vs Code Dev Container. If you have trouble building, try comparing the command your workflow uses to build to these.
### LINUX
## Configure
* Vs Code Plugin Command: `/usr/bin/cmake -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ --no-warn-unused-cli -S/workspaces/FITD -B/workspaces/FITD/build -G Ninja`
* Workflow Command: `cmake -B /home/runner/work/FITD/FITD/build -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Release -S /home/runner/work/FITD/FITD`
## Build
* Vs Code Plugin Command: `/usr/bin/cmake --build /workspaces/FITD/build --config Debug --target Fitd --`
* Workflow Command: `cmake --build /home/runner/work/FITD/FITD/build --config Release --verbose --target Fitd`
### WINDOWS
This 
## Configure
* Workflow Command: `cmake -B D:\a\FITD\FITD/build -DCMAKE_CXX_COMPILER=cl -DCMAKE_C_COMPILER=cl -DCMAKE_BUILD_TYPE=Release -S D:\a\FITD\FITD`
## Build
* Workflow Command: `cmake --build D:\a\FITD\FITD/build --config Release --verbose --target Fitd`