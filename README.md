# Content Aware Image Seam Carving
#### Exploration using [Qt5][qt5], [C++11][cpp], [Tup][tup], and a bit of [functional programing][esiegel_utility].

This is an implementation of [seam carving][seam_wiki], described in this [paper][seam_paper] and shown in this [video][seam_video].  Seam carving is an image processing algorithm to resize images by removing unimportant pixels in continuous seams.  This differs from standard resizing algorithms which typically just average neighboring pixels.  This technique can be very useful with images that have large similar sections, like a sky.

This is by no means the goto implementation; for that you should use the [cair library][cair].  For me, seam carving was alluring because it is relatively straight forward and also visually really cool.  Most importantly, it served as a platform for learning C++, Qt, and tup, all of which are things that I had zero experience with before this project.

## Running

After seamcarve is compiled, you can run the executable as follows:

```bash
build/seamcarve [-i PATH_TO_IMG]
```

## DEMO

![][demo]

## BUILDING

Originally, I had chosen cmake as the build tool, but I found it's magic not well suited for actually understanding the build process.  Eventually I chose [tup][tup].  Tup is extremely good at managing a dependency tree and knowing exactly which rules need reprocessing.  This makes incremental builds extremely easy, and makes compiling more tolerable.  Tup uses a FUSE file system to monitor changes to files.  This indirection, while usually helpful, can complicate things.

#### Installing tup on osx

```bash
brew install tup
chmod u+s $(which tup)
chown root $(which tup)
```

The last two lines are necessary in order to allow tup the ability to create chroot directories.  Refer to this [issue][tup_issue], which explains how chroot is used as a workaround to compiling with debug symbols.

#### Other dependencies

This project also depends on Qt5, boost, and a modern version of llvm/clang for compilation (Not all of these are hard requirements, just how the TupFile is currently defined).  You may also need to edit the TupFile so that your dependencies can be found.

```bash
brew install qt5
brew install boost
```

#### Compilation

Compiling the program by running:

```bash
tup init
tup
```

Looking at the TupFile you'll notice that I avoided the typical Qt workflow.  Instead of using the IDE QtCreator and qmake, I chose to use all its internals individually:
  
  * uic - compiles Qt UI files into cpp headers.  This header defines the gui layout.
  * moc - a meta compiler that transforms source with Qt's special syntax into implementation.  Used for slots and signals.
  * clang++ - a compiler with, among other things, awesome error messages.  Very useful when using templated code.  If you like, you can gcc without a problem.


[cpp]: http://en.cppreference.com/w/cpp
[cair]: http://sourceforge.net/projects/c-a-i-r/
[demo]: ../media/demo.gif?raw=true
[esiegel_utility]: https://github.com/esiegel/seamcarve/blob/master/include/utility.hpp#L72-81
[qt5]: http://qt-project.org/doc/qt-5/index.html
[seam_wiki]: http://en.wikipedia.org/wiki/Seam_carving
[seam_paper]: http://dl.acm.org/citation.cfm?id=1276390
[seam_video]: https://www.youtube.com/watch?v=vIFCV2spKtg
[tup]: http://gittup.org/tup/
[tup_issue]: https://github.com/gittup/tup/issues/195
