# txt

A shitty text editor

## Building

You will need:

* A linux machine
* gcc
* make

This is because the code utilises two header files that are not availabe on Windows:

* sys/ioctl.h
* termios.h

To build:

1. Download this repo
2. Enter the directory
3. Run `make` to create the binary
4. Run `make install` to copy the binary to path
5. Enjoy

## Features

1. It acts as a code editor as well
2. Syntax highlighting available in:
	- [`C`](https://en.wikipedia.org/wiki/C_(programming_language))
	- [`Python`](https://en.wikipedia.org/wiki/Python_(programming_language))
3. Search in file
4. Thats about it for now. It's WIP

## Updates

```markdown
* 1.0.0 Initial Release
```

## License

[GNU GPLv3](https://github.com/ItzAfroBoy/txt/blob/main/LICENSE)
