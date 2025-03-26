<!-- Template from https://github.com/othneildrew/Best-README-Template -->
<a id="readme-top"></a>


<!-- PROJECT LOGO -->
<div align="center">
  <h2 align="center">Asymmetric file encoder</h2>

  <p align="center">
    This program can encrypt any file using an asymmetric algorithm.
    <br />
  </p>
</div>


[![Github][github]][github-url]
![Python][python]


<!-- TABLE OF CONTENTS -->
<summary>Table of Contents</summary>
<ol>
  <li><a href="#getting-started">Getting started</a></li>
  <li><a href="#python-keygen-tool">Keygen tool</a></li>
  <li><a href="#licencing">Licencing</a></li>
</ol>


<!-- GETTING STARTED -->
## Getting started
Start by cloning this repository.
```sh
git clone 'https://github.com/AranweLTT/file-encoder.git'
```
Next you can build the program. You might need to install `build-essentials`.
```sh
g++ main.cpp lea9.cpp -o file-encoder
```
Once the program is built you are ready to use file-encoder !
```sh
file-encoder <input-file> <output-file> <mode> <key>
```

<u>Available modes:</u>
| Mode |             Description            |
|:----:|:----------------------------------:|
|  0   |  Classic encode/decode from key    |
|  1   |  Encode with additional pass       |
|  2   |  Decode with additional pass       |

## Python keygen tool
This Python script requires Numpy
```sh
python keygen9.py
```


<!-- LICENCE -->
## Licence
[![License: GPL v3][gpl3-badge]][gpl3-url]

This work is licensed under a GNU GPL v3 licence.


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[python]: images/badge/python.svg
[github]: images/badge/github.svg
[github-url]: https://github.com
[gpl3-url]: https://www.gnu.org/licenses/gpl-3.0
[gpl3-badge]: https://img.shields.io/badge/License-GPLv3-blue.svg
