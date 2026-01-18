
# Install instructions for WRText

## Instructions for Linux (Debian 12)

### Requisites
To install all the packages needed to compile WRText you can run the following command in your terminal:

`sudo apt update & sudo apt-get install libgtk-3-dev build-essential clang-format doxygen -y`

### Compiling
After downloading the repository enter the project root by typing `cd wrtext`.
Compile the source code by typing `make`. This will generate an executable in the root directory.
Execute `./WRText-v0.1` in the root directory to run the program.

### Installing
Type `make install` to install the program on your system.


## Instructions for Windows

idk

## Instructions for MacOS

### installation
First you need to install a package manager like homebrew
``` /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"```

Then clone the GitHub repository
```https://github.com/gabsimoni/wrtext.git```

And install the dependencies
```brew install gtk4 gcc```

### Compilation
Go in the folder containing the repository
```cd wrtext```

To compile the code you can use ```make``` or ```make debug``` for the debug mode.

To launch the application
```./WRText-v0.1``` 
