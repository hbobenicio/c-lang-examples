# Contributing

## Developtment Setup

### Meson and Ninja

```shell
# creating a python virtual environment for the project
python -m venv venv

# activating it in the current shell session.
# NOTE: all the rest of this document assumes you've ran this
. venv/bin/activate

# updates pip to the current version
pip install -U pip

# installs build dependencies from pip (meson and ninja ATM)
pip install -r dev-requirements.txt

# with meson installed now, setup the build directory
meson setup builddir
```

## Building

```shell
cd builddir
meson compile
```

## Testing

```shell
meson test
```
