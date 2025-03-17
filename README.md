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
  <li><a href="#python-keygen-tool">Getting started</a></li>
</ol>


<!-- GETTING STARTED -->
## Getting started
Start by cloning this repository.
```sh
git clone 'https://github.com/AranweLTT/file-encoder.git'
```
Next you can build the program
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
For fun I asked copilot to use my keygen script to find a key starting with a specific string. This is also known as a vanity key. I recommend using pypy if the substring you are looking for is more than 4 characters, or use the c++ port of this script.
```sh
pypy vanity-key.py cafe
```
This will try to find a key starting with 'cafe'. Note that because the key is hexadecimal only lower case hexadecimal characters can be searched for.

Here is an example of the c++ bulk tool:
```
Searching for vanity key from hexspeak dictionary...
Output format: <private-key>,<public-key>

3ca22d460ffe0f18da,badcafea336ac91288
da777fae169f7bfeca,badcafe9afacb231fa
```


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[python]: images/badge/python.svg
[github]: images/badge/github.svg
[github-url]: https://github.com
