<h1 align="center">
   <a href="http://adequate.biz">
      <img alt="Adequate Systems" src="https://raw.githubusercontent.com/adequatesystems/.github/main/media/adqlogo_banner.svg" /></a>
   <br/>Extended C/C++ Library<br/>
   <a href="https://github.com/adequatesystems/extended-c/actions/workflows/tests.yaml">
      <img src="https://github.com/adequatesystems/extended-c/actions/workflows/tests.yaml/badge.svg" alt="tests status" /></a>
   <a href="https://github.com/adequatesystems/extended-c/actions/workflows/codeql.yaml">
      <img src="https://github.com/adequatesystems/extended-c/actions/workflows/codeql.yaml/badge.svg" alt="codeql status" /></a>
   <a href="https://codecov.io/gh/adequatesystems/extended-c">
      <img src="https://codecov.io/gh/adequatesystems/extended-c/graph/badge.svg" alt="coverage status"></a>
   <br/>
   <a href="LICENSE.md">
      <img src="https://img.shields.io/badge/_License-MPL_2.0_Derivative-%23.svg?logoColor=lightgreen&logo=open%20source%20initiative&labelColor=2d3339&color=0059ff" alt="MPL 2.0 Derivative" /></a>
   <a href="https://github.com/adequatesystems/extended-c/releases">
      <img src="https://img.shields.io/github/release/adequatesystems/extended-c.svg?logo=semantic-release&labelColor=2d3339&label=Release&color=%230059ff" alt="release version"></a>
</h1>

Originally being derived from the Mochimo Codebase, this repository contains core C/C++ support intended for use in ongoing projects at Adequate Systems, LLC.

## Documentation
The [documentation](https://adequatesystems.github.io/extended-c/) contains the latest revision of the various support available in this Library.

## Usage
The Extended C/C++ Library was designed to be included in other projects as a [Git Submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules). For C projects utilizing a similar structure and makefile, it is recommended to add submodules to the `include/` directory of "superproject".

*If the "superproject" DOES NOT utilize a similar structure and makefile, you may have to include additional commands in your build process do "activate" submodule files.*

### Add Extended C as Submodule to project-repo
```sh
cd project-repo
git submodule add https://github.com/adequatesystems/extended-c include/extended-c
git commit -m "include extended-c submodule"
```

### Update Extended C Submodule to latest revision
```sh
cd project-repo/include/extended-c
git pull && git commit -m "update extended-c submodule to latest revision"
```

### Change Extended C Submodule to specific hash or version tag
```sh
cd project-repo/include/extended-c
git fetch
git checkout <hash or version tag>
git commit -m "checkout extended-c submodule to <hash or version tag>"
```

## License
This repository is licensed for use under an MPL 2.0 derivative Open Source license.  
The community is free to develop and change the code with the caveat that any changes must be for the benefit of the Mochimo cryptocurrency network (with a number of exclusions).  
Please read the [LICENSE](LICENSE.md) for more details on limitations and restrictions.
