# Recovery of jpegs


Recovers jpegs from an unreadable file of pictures, with memory blocks of 512 bytes each.

Jpeg image files always begin with a particular sequence of bytes. The first three bytes will be 0xffd8ff, while the fourth byte can be anywhere between 0xe0 and 0xef. Furthermore, the given file format stores jpegs in "blocks" of 512 bytes. This simplifies the search for the starting sequence somewhat, since the start of a jpeg must also be at the start of a block. Taking advantage of these facts, a file of unrecoverable jpegs, in the form of of pure binary, can be parsed in order to restore the jpeg files. 

The program simple reads through the binary in the file, looking for the start of a jpeg file sequence. Once found, it continually writes to a new jpeg image file until the next image is found. At this point, the process repeats itself until the end of file is found.

## Usage

The program, recover.c, has been compiled and can be used to recover image files from the provide binary file, card.raw. To do so, the folder containing the necessary files must first be navigated to in a terminal. Running "./recover &lt;filename&gt; will run the program on the target image file, whose name corresponds to &lt;filename&gt;.
The images will then be written to the same folder.