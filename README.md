# McCreight's Suffix Tree Algorithm

This repository contains a program that implements strict suffix tree
construction, inspired by the following:

```
Kurtz, S.
Reducing the space requirement of suffix trees.
Software-Practice and Experience
29(13):1149-1171, 1999
```

## Configuration

It is important to consider the configuration file `include/config.h`
before running the program. An incorrect situation may cause undefined
behavior.

## Usage

Using `make testrun` builds and queries patterns from two standard files.

In general, program is compiled via `make`. This generates the binary
`bin/mcc`, which is invoked with exactly one argument. This argument is assumed
to be the absolute path to a file, whose content is used to build a suffix
tree.

```
    ./bin/wotd TEXTFILE
```

## Acknowledgements

The initial commit `6346d8bcde34e77cc3c690c12ed33902eb818199 "Initial Commit"`
is taken directly as a part of the [mummer](https://github.com/mummer4/mummer)
project. All changes and modifications were under permission of the original
authors.
