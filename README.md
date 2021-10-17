# QStegano
Qt based Steganography application

### What is steganography? 
Steganography is the art of hiding the act of communitacion between two parties.

In digital world it comes to embedding some data inside media such as images, 
audio files or even network datagrams and then sending the prepared carrier.

### What can you find here?

In this project I would like to share my steganographic application. 
It deals with images and is able to hide text data inside them.
The core application is written in C++. 
For manipulating with images, I chose CImg library - fast and lightweight alternative to OpenCV.
The interface is written in Qt Widgets, it has a dark theme.
Features:
* hide & decode data in images
* configurable method: Basic LSB, XOR LSB, Inverted LSB or Optimal Pixel Adjustment Process
* configurable number of bits used (1, 2, 4, 8)

> **_NOTE:_**  Please keep in mind that the project and application serves educational purposes in the first place.
> The algorithms used and method proposed is not suitable for any production / operational applications yet.

### Screenshots
![Hiding & decoding process](demo.gif)

### Instalation & configuration
```
git clone git@github.com:palucki/QStegano.git
cd QStegano
qmake QStegano.pro
build (platform / IDE dependent)
```

### Author

* **Piotr Pałucki** - [palucki](https://palucki.github.io)
