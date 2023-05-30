<p align="center">
  <h1 align="center">py4pd</h1>
  <h4 align="center">Python in the PureData environment.</h4>
</p>
<p align="center">
    <a href="https://github.com/charlesneimog/py4pd/releases/latest"><img src="https://img.shields.io/github/release/charlesneimog/py4pd.svg?include_prereleases" alt="Release"></a>
    <a href="https://github.com/plugdata-team/plugdata/blob/main/LICENSE"><img src="https://img.shields.io/badge/license-GPL--v3-blue.svg" alt="License"></a>
    <a href="https://zenodo.org/record/7677265"><img src="https://zenodo.org/badge/doi/10.5281/zenodo.7677265.svg" alt="DOI"></a>
</p>
    
<p align="center">
  <a href="https://github.com/charlesneimog/py4pd/actions/workflows/Builder.yml"><img src="https://github.com/charlesneimog/py4pd/actions/workflows/Builder.yml/badge.svg?branch=develop"></a>
</p>

`py4pd` allows the use of Python within PureData. It has the same goal as [py/pyext](https://github.com/grrrr/py), being simpler to keep up to date, use modules, use lists, and other things.  with Python you can:
* Use scores inside PureData;
* Use svg/draws as scores;
* OpenMusic functions in libraries like `om_py`, `music21`, `neoscore`, and others;
* Sound analisys with `magenta`, `librosa`, and `pyAudioAnalaysis`;

Go to [Docs](https://www.charlesneimog.com/py4pd) to see examples.

### Download py4pd
1. Open PureData, `Help->Find Externals->` search for `py4pd` then select py4pd and click on `Install`: 
2. ⚠️ For now, you must install Python ⚠️.

* **Linux**: `sudo dnf install python3.11` or `sudo apt install python3.11`;
* **MacOS**: Go to https://www.python.org/downloads/release/python-3112/ and install normally.
* **Windows**: Go to https://www.python.org/downloads/release/python-3112/ and install normally.

## Wiki | Instructions for use

* Go to [Docs](https://www.charlesneimog.com/py4pd).

## For Developers

Just one thing, the development of this object occurs in de `develop` branch, the main branch corresponds to the last release available in `Deken`.

## Building from Source

* To build from the source code, you can follow the steps: [Compiling from Source Code.](https://github.com/charlesneimog/py4pd/blob/master/resources/BUILD.md).

## Citing this work

Consider citing this repository, on the right side of the page, below of about.

```APA
Neimog, C. K. (2023). py4pd, Python inside PureData (Version 0.5.0) [Computer software]. https://doi.org/10.5281/zenodo.7677265
```
``` bibtex
@software{Neimog_py4pd_Python_inside_2023,
author = {Neimog, Charles K.},
doi = {10.5281/zenodo.7677265},
license = {GPL3},
month = {2},
title = {{py4pd, Python inside PureData}},
url = {https://github.com/charlesneimog/py4pd},
version = {0.5.0},
year = {2023}
}
```

