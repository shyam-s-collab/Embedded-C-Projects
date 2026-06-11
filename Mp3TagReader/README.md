# MP3 Tag Reader

Command-line ID3 tag reader and editor built from the provided skeleton.

## Build

```sh
gcc *.c -o mp3tagreader
```

or:

```sh
make
```

## Usage

```sh
./mp3tagreader -h
./mp3tagreader -v song.mp3
./mp3tagreader -e -t song.mp3 "New title"
./mp3tagreader -e -a song.mp3 "New artist"
./mp3tagreader -e -A song.mp3 "New album"
./mp3tagreader -e -y song.mp3 "2026"
./mp3tagreader -e -c song.mp3 "New comment"
./mp3tagreader -e -g song.mp3 "Genre"
```

`-T` is also accepted as an artist-edit alias.

## Supported Tags

- View: ID3v1, ID3v2.2, ID3v2.3, and ID3v2.4 text metadata.
- Edit: ID3v1/ID3v2.2/ID3v2.3 files are rewritten as ID3v2.3.
- Edit: ID3v2.4 files are rewritten as ID3v2.4 with syncsafe frame sizes and `TDRC` year/date frames.
