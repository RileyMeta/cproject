# CProject
[![License: GPL](https://img.shields.io/badge/License-GPL-GREEN.svg)](https://opensource.org/licenses/MIT)

Create a formatted C project folder.

## Description
Create a minimal setup of a C Project folder, with a `README.md`, generic `Makefile`, and a simple `main.c` that will print `"hello world!"` to the console to prove it works.

### Folder Structure
```tree
.
└── project/
    ├── src/
    │   └── main.c
    ├── include/
    ├── lib/
    ├── Makefile
    └── README.md
```

## Install
### Prerequisites
- C compiler (e.g., `gcc`)
- Make (optional)

### Clone
```bash
git clone https://github.com/RileyMeta/cproject.git
cd cproject
```

### Automatic
```bash
make
make install
```

### Manual
```bash
gcc src/main.c src/backend.c -o cproject -Wall -std=c11 -Wno-missing-braces
sudo cp cproject /usr/bin/cproject
sudo chmod a+x /usr/bin/cproject
```

## Uninstall
### Automatic
```bash
make uninstall
```

### Manual
```bash
sudo rm -rf $(which cproject)
```

## Usage
```bash
cproject project_name
```
Alternatively you can generate just a `Makefile` with:
```bash
cproject -M project_name
```

### Options
| short flag | long flag  | description                             |
|------------|------------|-----------------------------------------|
| -M         | --makefile | Create only the makefile for a proejct. |
| -I         | --inplace  | Use the folder as-is and move files.    |

---
## License
This project is licensed under the GPL License - see the [LICENSE](LICENSE) file for details.
