Sylvan
==========


<img src="https://github.com/EterCyber/Sylvan/blob/master/projects/gui/res/icons/app.ico"/>

Sylvan is a graphical user interface, command line interface, and Chinese Chess (also called Xiangqi) library
derived from [Cute Chess](https://github.com/cutechess/cutechess). Sylvan is written in C++ using the [Qt
framework](https://www.qt.io/).

Installing
----------

Binaries for both the GUI and the `sylvan-cli` command-line interface are available [here](https://github.com/EterCyber/Sylvan/releases)

Compiling
---------

Sylvan requires Qt 5.7 or greater, a compiler with C++11 support and `qmake`.

In the simplest case you only have to issue:

    $ qmake
    $ make

If you are using the Visual C++ compiler replace `make` with `nmake`.

Documentation is available as Unix manual pages in the `docs/` directory. API
documentation can be built by issuing `make doc-api` (requires [Doxygen](http://www.doxygen.org/)).

For detailed build instruction on various operating systems please visit:
* [Making a release](https://github.com/EterCyber/Sylvan/wiki/Making-a-release)

Running
-------

The `sylvan-cli` program is run from the command line to play games between
chess engines. For example to play ten games between two Sloppy engines
(assuming `sloppy` is in PATH) with a time control of 40 moves in 60
seconds:

    $ sylvan-cli -engine cmd=sloppy -engine cmd=sloppy -each proto=uci tc=40/60 -rounds 10

See `sylvan-cli -help` for descriptions of the supported options or manuals
for full documentation.

License
-------

Sylvan is released under the GPLv3+ license except for the components in
the `projects/lib/components` and `projects/gui/components` directories which
are released under the MIT License.

Credits
-------

Sylvan was written by Wilbert Lee and [contributors](https://github.com/EterCyber/Sylvan/graphs/contributors)

