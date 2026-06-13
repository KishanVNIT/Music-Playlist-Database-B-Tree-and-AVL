# Music Playlist Manager (B-Tree Based) — C

A console-based music playlist management system implemented in **C**, using a **B-Tree (order T=3)** as the core data structure for storing and organizing songs. Supports multiple playlists, set operations between playlists, playback history (doubly linked list), shuffle/repeat modes, and persistence to a file.

## Overview

Each song (`Song`) has a `songID`, `title`, and `artist`. Songs are stored in a B-Tree (`SongNode`) keyed primarily by `songID`, which provides efficient insertion, deletion, search, and range queries. Secondary B-Trees can be built on the fly (sorted by title, artist, or artist+title) for alternate display/search orderings.

Up to **10 playlists** can be managed simultaneously, each backed by its own B-Tree. Playback history is tracked using a doubly linked list, allowing chronological and reverse-chronological viewing.

## Data Structures Used

- **B-Tree (order T = 3)** — primary structure for each playlist; keys sorted by `songID`. Supports full insert, search, and delete (with borrow/merge balancing).
- **Secondary B-Trees** — built temporarily by traversing the primary tree, sorted by:
  - Title
  - Artist
  - Artist + Title (composite key)
- **Doubly Linked List (`HistoryNode`)** — stores playback history in order, supporting both forward and backward traversal.
- **Explicit stack (array-based frames)** — used for iterative inorder traversal when saving playlists to file.
- **Flat array + Fisher-Yates shuffle** — used for shuffle mode.

## Features

| Menu Option | Description |
|---|---|
| 1. Add a song | Add a song (by ID, title, artist) to a new or existing playlist. Rejects duplicate IDs. |
| 2. Delete a song | Remove a song by ID from a chosen playlist using full B-Tree deletion. |
| 3. Display Playlist | Display a playlist sorted by Title, Artist, or Artist+Title. |
| 4. Search a song | Search within a playlist by Song ID, Title, or Artist. |
| 5. Shuffle Mode | Display all songs in a playlist in random order (Fisher-Yates shuffle). |
| 6. Repeat Mode | Continuously replay a playlist (with periodic prompt to continue or stop). |
| 7. Range Search | List all songs whose ID falls within a given `[id1, id2]` range. |
| 8. Play a Playlist | Play (display) every song in a playlist in ID order and log each to history. |
| 9. Play a song | Play a single song by ID and log it to history. |
| 10. Display History | View playback history chronologically or in reverse. |
| 11. Operations on multiple playlists | Combine two playlists via Union, Intersection, Difference, or Symmetric Difference — result is stored as a new playlist. |
| 12. Exit | Save all playlists to file and free all allocated memory. |

## Set Operations on Playlists

When at least 2 playlists exist, you can combine any two of them (results stored as a new playlist, up to a max of 10 total):

- **Union** — all songs from both playlists (duplicates by ID removed).
- **Intersection** — songs present in both playlists.
- **Difference** — songs in Playlist 1 but not in Playlist 2.
- **Symmetric Difference** — songs in either playlist but not in both.

## Persistence

- On startup, the program attempts to load playlists from `playlists.txt`.
- On exit (option 12), all playlists are saved back to `playlists.txt` using an iterative (stack-based) inorder traversal of each B-Tree.
- File format: first line is the playlist count, followed by one line per song: `<playlistIndex> <songID> <title> <artist>`.

> **Note:** Titles and artists are read with `scanf("%s", ...)`, so multi-word titles/artists containing spaces are **not** supported as written.

## How to Build & Run

```bash
gcc -o MusicPlayListBTree MusicPlayListBTree.c
./MusicPlayListBTree
```

## B-Tree Configuration

| Constant | Value | Meaning |
|---|---|---|
| `T` | 3 | B-Tree minimum degree — each node holds between `T-1` and `2T-1` (i.e., 2–5) keys |

## Notes

- The B-Tree is the single source of truth for each playlist; secondary orderings (by title/artist) are built as temporary trees and freed after use.
- `searchByID` returns a heap-allocated single-key node — callers are responsible for `free()`-ing it after use.
- Memory for all playlists and the history list is freed on exit (option 12).
- This is an educational/demo console application with no external dependencies beyond the C standard library.

## License
This project is for educational purposes and is free to use or modify.
