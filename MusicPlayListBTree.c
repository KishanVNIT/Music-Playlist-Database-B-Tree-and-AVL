// Music Playlist System using B-Tree
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define T 3  // Minimum degree of B-tree (each node has at most 2T-1 keys, at least T-1 keys except root)

// ─────────────────────────────────────────────
//  Data Structures
// ─────────────────────────────────────────────

// A single song record stored as a key inside a B-tree node
typedef struct Song {
    int   songID;
    char  title[100];
    char  artist[100];
} Song;

// B-tree node: holds up to 2T-1 songs and up to 2T child pointers
typedef struct SongNode {
    Song            keys[2 * T - 1];      // Song records stored in this node
    struct SongNode *children[2 * T];     // Child pointers (NULL for leaves)
    int             n;                    // Current number of keys in this node
    bool            leaf;                 // True if this is a leaf node
} SongNode;

// History doubly-linked list node (unchanged)
typedef struct HistoryNode {
    int  songID;
    char title[100];
    char artist[100];
    struct HistoryNode *prev;
    struct HistoryNode *next;
} HistoryNode;

// ─────────────────────────────────────────────
//  B-tree Node Creation
// ─────────────────────────────────────────────

SongNode *createNode(bool leaf)
{
    SongNode *node = (SongNode *)malloc(sizeof(SongNode));
    node->n    = 0;
    node->leaf = leaf;
    for (int i = 0; i < 2 * T; i++)
        node->children[i] = NULL;
    return node;
}

// ─────────────────────────────────────────────
//  Free an entire B-tree
// ─────────────────────────────────────────────

void freeTree(SongNode *root)
{
    if (!root) return;
    if (!root->leaf)
        for (int i = 0; i <= root->n; i++)
            freeTree(root->children[i]);
    free(root);
}

// ─────────────────────────────────────────────
//  Inorder traversal: prints all songs sorted by the tree's key
// ─────────────────────────────────────────────

void inorder(SongNode *node)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            inorder(node->children[i]);
        printf("[ID: %d, Title: %s, Artist: %s] -> ",
               node->keys[i].songID,
               node->keys[i].title,
               node->keys[i].artist);
    }
    if (!node->leaf)
        inorder(node->children[i]);
}

// ─────────────────────────────────────────────
//  Split child  (standard B-tree split)
// ─────────────────────────────────────────────

void splitChild(SongNode *parent, int idx, SongNode *child)
{
    // Create new node to hold the right half of child's keys
    SongNode *newNode = createNode(child->leaf);
    newNode->n = T - 1;

    // Copy the right T-1 keys of child into newNode
    for (int j = 0; j < T - 1; j++)
        newNode->keys[j] = child->keys[j + T];

    // Copy the right T children of child into newNode (if not leaf)
    if (!child->leaf)
        for (int j = 0; j < T; j++)
            newNode->children[j] = child->children[j + T];

    child->n = T - 1;   // Reduce key count of child

    // Shift parent's children to make room for newNode
    for (int j = parent->n; j >= idx + 1; j--)
        parent->children[j + 1] = parent->children[j];
    parent->children[idx + 1] = newNode;

    // Shift parent's keys to make room for the middle key
    for (int j = parent->n - 1; j >= idx; j--)
        parent->keys[j + 1] = parent->keys[j];

    // Move the middle key of child up to parent
    parent->keys[idx] = child->keys[T - 1];
    parent->n++;
}

// ─────────────────────────────────────────────
//  Insert into a non-full node (by songID)
// ─────────────────────────────────────────────

static void insertNonFullByID(SongNode *node, Song song)
{
    int i = node->n - 1;

    if (node->leaf) {
        // Shift keys right to find insertion position
        while (i >= 0 && node->keys[i].songID > song.songID)
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = song;
        node->n++;
    } else {
        // Find child to descend into
        while (i >= 0 && node->keys[i].songID > song.songID)
            i--;
        i++;
        if (node->children[i]->n == 2 * T - 1) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i].songID < song.songID)
                i++;
        }
        insertNonFullByID(node->children[i], song);
    }
}

// ─────────────────────────────────────────────
//  Public addSong: insert a song by songID
//  Returns the (possibly new) root.
//  *status = 1 on success, 0 if duplicate ID.
// ─────────────────────────────────────────────

// Forward declaration
SongNode *searchNodeByID(SongNode *node, int id);

SongNode *addSong(SongNode *root, int id, char t[], char a[], int *status)
{
    // Check for duplicate ID
    if (searchNodeByID(root, id) != NULL) {
        *status = 0;
        return root;
    }

    Song song;
    song.songID = id;
    strncpy(song.title,  t, 99); song.title[99]  = '\0';
    strncpy(song.artist, a, 99); song.artist[99] = '\0';

    // If root is NULL, create the first node
    if (root == NULL) {
        root = createNode(true);
        root->keys[0] = song;
        root->n = 1;
        *status = 1;
        return root;
    }

    // If root is full, grow the tree upward
    if (root->n == 2 * T - 1) {
        SongNode *newRoot = createNode(false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFullByID(newRoot, song);
        *status = 1;
        return newRoot;
    }

    insertNonFullByID(root, song);
    *status = 1;
    return root;
}

// ─────────────────────────────────────────────
//  searchNodeByID: returns pointer to the Song inside the tree,
//  or NULL if not found.  (We return the whole SongNode for compatibility.)
// ─────────────────────────────────────────────

SongNode *searchNodeByID(SongNode *node, int id)
{
    if (!node) return NULL;

    int i = 0;
    while (i < node->n && id > node->keys[i].songID)
        i++;

    if (i < node->n && node->keys[i].songID == id)
        return node;   // Found in this node

    if (node->leaf)
        return NULL;   // Not found

    return searchNodeByID(node->children[i], id);
}

// ─────────────────────────────────────────────
//  searchByID: returns a heap-allocated SongNode* containing just
//  the found Song (caller must free), or NULL.
//  (Kept same interface as AVL version for handle functions.)
// ─────────────────────────────────────────────

SongNode *searchByID(SongNode *root, int id)
{
    if (id <= 0 || !root) return NULL;

    SongNode *found = searchNodeByID(root, id);
    if (!found) return NULL;

    // Find the key inside the node
    int i = 0;
    while (i < found->n && found->keys[i].songID != id) i++;

    // Return a temporary single-key node so caller can access song info
    // We return pointer to found node; caller uses keys[i] at position
    // BUT since original code used song->songID / song->title / song->artist,
    // we return a small allocated node with just that one key.
    SongNode *result = createNode(true);
    result->keys[0] = found->keys[i];
    result->n = 1;
    return result;  // Caller must free this
}

// ─────────────────────────────────────────────
//  searchByTitle: print all songs with matching title (inorder traversal)
// ─────────────────────────────────────────────

void searchByTitle(SongNode *node, char title[])
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            searchByTitle(node->children[i], title);
        if (strcmp(title, node->keys[i].title) == 0)
            printf("[ID: %d, Title: %s, Artist: %s] -> ",
                   node->keys[i].songID,
                   node->keys[i].title,
                   node->keys[i].artist);
    }
    if (!node->leaf)
        searchByTitle(node->children[i], title);
}

// ─────────────────────────────────────────────
//  searchByArtist: print all songs with matching artist
// ─────────────────────────────────────────────

void searchByArtist(SongNode *node, char artist[])
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            searchByArtist(node->children[i], artist);
        if (strcmp(artist, node->keys[i].artist) == 0)
            printf("[ID: %d, Title: %s, Artist: %s] -> ",
                   node->keys[i].songID,
                   node->keys[i].title,
                   node->keys[i].artist);
    }
    if (!node->leaf)
        searchByArtist(node->children[i], artist);
}

// ─────────────────────────────────────────────
//  rangeSearch: print all songs whose ID is in [s1, s2]
// ─────────────────────────────────────────────

void rangeSearch(SongNode *node, int s1, int s2)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        // Descend into child[i] if it may contain keys >= s1
        if (!node->leaf && s1 < node->keys[i].songID)
            rangeSearch(node->children[i], s1, s2);

        if (node->keys[i].songID >= s1 && node->keys[i].songID <= s2)
            printf("[ID: %d, Title: %s, Artist: %s] -> ",
                   node->keys[i].songID,
                   node->keys[i].title,
                   node->keys[i].artist);

        // Stop early if we've passed s2
        if (node->keys[i].songID > s2) return;
    }
    // Descend into last child if needed
    if (!node->leaf)
        rangeSearch(node->children[i], s1, s2);
}

// ─────────────────────────────────────────────
//  B-tree Delete Helpers
// ─────────────────────────────────────────────

// Find index of first key >= id in node
static int findKeyIdx(SongNode *node, int id)
{
    int idx = 0;
    while (idx < node->n && node->keys[idx].songID < id)
        idx++;
    return idx;
}

// Remove key at position idx from a leaf
static void removeFromLeaf(SongNode *node, int idx)
{
    for (int i = idx + 1; i < node->n; i++)
        node->keys[i - 1] = node->keys[i];
    node->n--;
}

// Get in-order predecessor key (rightmost key in left subtree of keys[idx])
static Song getPredecessor(SongNode *node, int idx)
{
    SongNode *curr = node->children[idx];
    while (!curr->leaf)
        curr = curr->children[curr->n];
    return curr->keys[curr->n - 1];
}

// Get in-order successor key (leftmost key in right subtree of keys[idx])
static Song getSuccessorKey(SongNode *node, int idx)
{
    SongNode *curr = node->children[idx + 1];
    while (!curr->leaf)
        curr = curr->children[0];
    return curr->keys[0];
}

// Merge child[idx] and child[idx+1] into child[idx] (child[idx+1] freed)
static void mergeChildren(SongNode *node, int idx)
{
    SongNode *left  = node->children[idx];
    SongNode *right = node->children[idx + 1];

    // Pull down the separator key from parent into left
    left->keys[left->n] = node->keys[idx];

    // Copy children of right into left (if not leaf)
    if (!left->leaf)
        left->children[left->n + 1] = right->children[0];

    // Copy keys of right into left
    for (int i = 0; i < right->n; i++) {
        left->keys[left->n + 1 + i] = right->keys[i];
        if (!left->leaf)
            left->children[left->n + 2 + i] = right->children[i + 1];
    }

    left->n += right->n + 1;

    // Remove separator key from parent and shift keys/children left
    for (int i = idx + 1; i < node->n; i++)
        node->keys[i - 1] = node->keys[i];
    for (int i = idx + 2; i <= node->n; i++)
        node->children[i - 1] = node->children[i];

    node->n--;
    free(right);
}

// Borrow from left sibling: child[idx] borrows from child[idx-1]
static void borrowFromPrev(SongNode *node, int idx)
{
    SongNode *child   = node->children[idx];
    SongNode *sibling = node->children[idx - 1];

    // Shift child's keys right by 1
    for (int i = child->n - 1; i >= 0; i--)
        child->keys[i + 1] = child->keys[i];
    if (!child->leaf)
        for (int i = child->n; i >= 0; i--)
            child->children[i + 1] = child->children[i];

    // Bring down separator from parent into child
    child->keys[0] = node->keys[idx - 1];
    if (!child->leaf)
        child->children[0] = sibling->children[sibling->n];

    // Move last key of sibling up to parent
    node->keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n++;
    sibling->n--;
}

// Borrow from right sibling: child[idx] borrows from child[idx+1]
static void borrowFromNext(SongNode *node, int idx)
{
    SongNode *child   = node->children[idx];
    SongNode *sibling = node->children[idx + 1];

    // Bring down separator from parent into child's last position
    child->keys[child->n] = node->keys[idx];
    if (!child->leaf)
        child->children[child->n + 1] = sibling->children[0];

    // Move first key of sibling up to parent
    node->keys[idx] = sibling->keys[0];

    // Shift sibling's keys left by 1
    for (int i = 1; i < sibling->n; i++)
        sibling->keys[i - 1] = sibling->keys[i];
    if (!sibling->leaf)
        for (int i = 1; i <= sibling->n; i++)
            sibling->children[i - 1] = sibling->children[i];

    child->n++;
    sibling->n--;
}

// Ensure child[idx] has at least T keys; borrow or merge as needed
static void fill(SongNode *node, int idx)
{
    if (idx > 0 && node->children[idx - 1]->n >= T)
        borrowFromPrev(node, idx);
    else if (idx < node->n && node->children[idx + 1]->n >= T)
        borrowFromNext(node, idx);
    else {
        if (idx < node->n)
            mergeChildren(node, idx);
        else
            mergeChildren(node, idx - 1);
    }
}

// Core recursive delete (by songID)
static SongNode *deleteHelper(SongNode *node, int id, int *status)
{
    if (!node) return NULL;

    int idx = findKeyIdx(node, id);

    if (idx < node->n && node->keys[idx].songID == id) {
        // Key found in this node
        *status = 1;
        if (node->leaf) {
            removeFromLeaf(node, idx);
        } else {
            if (node->children[idx]->n >= T) {
                // Replace with in-order predecessor
                Song pred = getPredecessor(node, idx);
                node->keys[idx] = pred;
                node->children[idx] = deleteHelper(node->children[idx], pred.songID, status);
            } else if (node->children[idx + 1]->n >= T) {
                // Replace with in-order successor
                Song succ = getSuccessorKey(node, idx);
                node->keys[idx] = succ;
                node->children[idx + 1] = deleteHelper(node->children[idx + 1], succ.songID, status);
            } else {
                // Merge children[idx] and children[idx+1]
                mergeChildren(node, idx);
                node->children[idx] = deleteHelper(node->children[idx], id, status);
            }
        }
    } else {
        // Key not in this node
        if (node->leaf) {
            // Key not found in the tree
            return node;
        }

        bool isLast = (idx == node->n);

        if (node->children[idx]->n < T)
            fill(node, idx);

        // After fill, idx may shift if we merged with left sibling
        if (isLast && idx > node->n)
            node->children[idx - 1] = deleteHelper(node->children[idx - 1], id, status);
        else
            node->children[idx] = deleteHelper(node->children[idx], id, status);
    }

    return node;
}

// ─────────────────────────────────────────────
//  Public deleteSong
// ─────────────────────────────────────────────

SongNode *deleteSong(SongNode *root, int id, int *status)
{
    if (id <= 0 || !root) return root;

    root = deleteHelper(root, id, status);

    // If root is now empty and has a child, shrink the tree
    if (root && root->n == 0 && !root->leaf) {
        SongNode *oldRoot = root;
        root = root->children[0];
        free(oldRoot);
    }

    return root;
}

// ─────────────────────────────────────────────
//  Build secondary B-trees (for display by title / artist)
// ─────────────────────────────────────────────

// Insert into a B-tree sorted by title (secondary key: songID)
static void insertNonFullByTitle(SongNode *node, Song song);

static SongNode *addSongByTitle(SongNode *root, Song song)
{
    if (root == NULL) {
        root = createNode(true);
        root->keys[0] = song;
        root->n = 1;
        return root;
    }

    if (root->n == 2 * T - 1) {
        SongNode *newRoot = createNode(false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFullByTitle(newRoot, song);
        return newRoot;
    }

    insertNonFullByTitle(root, song);
    return root;
}

static void insertNonFullByTitle(SongNode *node, Song song)
{
    int i = node->n - 1;

    if (node->leaf) {
        while (i >= 0) {
            int cmp = strcmp(node->keys[i].title, song.title);
            if (cmp > 0 || (cmp == 0 && node->keys[i].songID > song.songID)) {
                node->keys[i + 1] = node->keys[i];
                i--;
            } else break;
        }
        node->keys[i + 1] = song;
        node->n++;
    } else {
        while (i >= 0) {
            int cmp = strcmp(node->keys[i].title, song.title);
            if (cmp > 0 || (cmp == 0 && node->keys[i].songID > song.songID))
                i--;
            else break;
        }
        i++;
        if (node->children[i]->n == 2 * T - 1) {
            splitChild(node, i, node->children[i]);
            int cmp = strcmp(node->keys[i].title, song.title);
            if (cmp < 0 || (cmp == 0 && node->keys[i].songID < song.songID))
                i++;
        }
        insertNonFullByTitle(node->children[i], song);
    }
}

// Insert into a B-tree sorted by artist (secondary key: songID)
static void insertNonFullByArtist(SongNode *node, Song song);

static SongNode *addSongByArtist(SongNode *root, Song song)
{
    if (root == NULL) {
        root = createNode(true);
        root->keys[0] = song;
        root->n = 1;
        return root;
    }

    if (root->n == 2 * T - 1) {
        SongNode *newRoot = createNode(false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFullByArtist(newRoot, song);
        return newRoot;
    }

    insertNonFullByArtist(root, song);
    return root;
}

static void insertNonFullByArtist(SongNode *node, Song song)
{
    int i = node->n - 1;

    if (node->leaf) {
        while (i >= 0) {
            int cmp = strcmp(node->keys[i].artist, song.artist);
            if (cmp > 0 || (cmp == 0 && node->keys[i].songID > song.songID)) {
                node->keys[i + 1] = node->keys[i];
                i--;
            } else break;
        }
        node->keys[i + 1] = song;
        node->n++;
    } else {
        while (i >= 0) {
            int cmp = strcmp(node->keys[i].artist, song.artist);
            if (cmp > 0 || (cmp == 0 && node->keys[i].songID > song.songID))
                i--;
            else break;
        }
        i++;
        if (node->children[i]->n == 2 * T - 1) {
            splitChild(node, i, node->children[i]);
            int cmp = strcmp(node->keys[i].artist, song.artist);
            if (cmp < 0 || (cmp == 0 && node->keys[i].songID < song.songID))
                i++;
        }
        insertNonFullByArtist(node->children[i], song);
    }
}

// Insert into a B-tree sorted by (artist, title) composite key
static void insertNonFullByArtistAndTitle(SongNode *node, Song song);

static SongNode *addSongByArtistAndTitle(SongNode *root, Song song)
{
    if (root == NULL) {
        root = createNode(true);
        root->keys[0] = song;
        root->n = 1;
        return root;
    }

    if (root->n == 2 * T - 1) {
        SongNode *newRoot = createNode(false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFullByArtistAndTitle(newRoot, song);
        return newRoot;
    }

    insertNonFullByArtistAndTitle(root, song);
    return root;
}

// Compare two songs by (artist, title, songID)
static int cmpArtistTitle(Song *a, Song *b)
{
    int c = strcmp(a->artist, b->artist);
    if (c != 0) return c;
    c = strcmp(a->title, b->title);
    if (c != 0) return c;
    return a->songID - b->songID;
}

static void insertNonFullByArtistAndTitle(SongNode *node, Song song)
{
    int i = node->n - 1;

    if (node->leaf) {
        while (i >= 0 && cmpArtistTitle(&node->keys[i], &song) > 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = song;
        node->n++;
    } else {
        while (i >= 0 && cmpArtistTitle(&node->keys[i], &song) > 0)
            i--;
        i++;
        if (node->children[i]->n == 2 * T - 1) {
            splitChild(node, i, node->children[i]);
            if (cmpArtistTitle(&node->keys[i], &song) < 0)
                i++;
        }
        insertNonFullByArtistAndTitle(node->children[i], song);
    }
}

// ─────────────────────────────────────────────
//  Build secondary trees by traversing primary (ID-ordered) tree
// ─────────────────────────────────────────────

void buildByID(SongNode **dst, SongNode *src, int *status)
{
    if (!src) return;
    int i;
    for (i = 0; i < src->n; i++) {
        if (!src->leaf)
            buildByID(dst, src->children[i], status);
        *dst = addSong(*dst, src->keys[i].songID,
                       src->keys[i].title, src->keys[i].artist, status);
    }
    if (!src->leaf)
        buildByID(dst, src->children[i], status);
}

void buildByTitle(SongNode **dst, SongNode *src)
{
    if (!src) return;
    int i;
    for (i = 0; i < src->n; i++) {
        if (!src->leaf)
            buildByTitle(dst, src->children[i]);
        *dst = addSongByTitle(*dst, src->keys[i]);
    }
    if (!src->leaf)
        buildByTitle(dst, src->children[i]);
}

void buildByArtist(SongNode **dst, SongNode *src)
{
    if (!src) return;
    int i;
    for (i = 0; i < src->n; i++) {
        if (!src->leaf)
            buildByArtist(dst, src->children[i]);
        *dst = addSongByArtist(*dst, src->keys[i]);
    }
    if (!src->leaf)
        buildByArtist(dst, src->children[i]);
}

void buildByArtistAndTitle(SongNode **dst, SongNode *src)
{
    if (!src) return;
    int i;
    for (i = 0; i < src->n; i++) {
        if (!src->leaf)
            buildByArtistAndTitle(dst, src->children[i]);
        *dst = addSongByArtistAndTitle(*dst, src->keys[i]);
    }
    if (!src->leaf)
        buildByArtistAndTitle(dst, src->children[i]);
}

// ─────────────────────────────────────────────
//  Shuffle helpers (collect all song pointers into a flat array)
// ─────────────────────────────────────────────

void countSongs(SongNode *node, int *count)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            countSongs(node->children[i], count);
        (*count)++;
    }
    if (!node->leaf)
        countSongs(node->children[i], count);
}

// Store pointers to Song structs inside tree nodes into arr[]
void storeSongs(SongNode *node, Song *arr[], int *idx)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            storeSongs(node->children[i], arr, idx);
        arr[*idx] = &node->keys[i];
        (*idx)++;
    }
    if (!node->leaf)
        storeSongs(node->children[i], arr, idx);
}

void shufflePlaylist(SongNode *root)
{
    int n = 0;
    countSongs(root, &n);
    if (n == 0) { printf("\nPlaylist is empty.\n"); return; }

    Song *arr[n];
    int start = 0;
    storeSongs(root, arr, &start);

    // Fisher-Yates shuffle
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Song *tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
    }

    printf("\n");
    for (int i = 0; i < n; i++)
        printf("[ID: %d, Title: %s, Artist: %s] -> ",
               arr[i]->songID, arr[i]->title, arr[i]->artist);
    printf("NULL\n");
}

// ─────────────────────────────────────────────
//  Set operations (union / intersection / difference)
// ─────────────────────────────────────────────

void buildUnion(SongNode **dst, SongNode *src, int *status)
{
    if (!src) return;
    int i;
    for (i = 0; i < src->n; i++) {
        if (!src->leaf)
            buildUnion(dst, src->children[i], status);
        *dst = addSong(*dst, src->keys[i].songID,
                       src->keys[i].title, src->keys[i].artist, status);
    }
    if (!src->leaf)
        buildUnion(dst, src->children[i], status);
}

SongNode *unionOfPlaylist(SongNode *p1, SongNode *p2)
{
    SongNode *result = NULL;
    int status = 0;
    if (!p1) { buildByID(&result, p2, &status); return result; }
    if (!p2) { buildByID(&result, p1, &status); return result; }
    buildByID(&result, p1, &status);
    buildUnion(&result, p2, &status);
    return result;
}

void buildIntersection(SongNode **result, SongNode *p1, SongNode *p2, int *status)
{
    if (!p1) return;
    int i;
    for (i = 0; i < p1->n; i++) {
        if (!p1->leaf)
            buildIntersection(result, p1->children[i], p2, status);
        if (searchNodeByID(p2, p1->keys[i].songID))
            *result = addSong(*result, p1->keys[i].songID,
                              p1->keys[i].title, p1->keys[i].artist, status);
    }
    if (!p1->leaf)
        buildIntersection(result, p1->children[i], p2, status);
}

SongNode *intersectionOfPlaylist(SongNode *p1, SongNode *p2)
{
    SongNode *result = NULL;
    if (!p1 || !p2) return result;
    int status = 0;
    buildIntersection(&result, p1, p2, &status);
    return result;
}

void buildDifference(SongNode **result, SongNode *p1, SongNode *p2, int *status)
{
    if (!p1) return;
    int i;
    for (i = 0; i < p1->n; i++) {
        if (!p1->leaf)
            buildDifference(result, p1->children[i], p2, status);
        if (!searchNodeByID(p2, p1->keys[i].songID))
            *result = addSong(*result, p1->keys[i].songID,
                              p1->keys[i].title, p1->keys[i].artist, status);
    }
    if (!p1->leaf)
        buildDifference(result, p1->children[i], p2, status);
}

SongNode *differenceOfPlaylist(SongNode *p1, SongNode *p2)
{
    SongNode *result = NULL;
    int status = 0;
    if (!p1) return result;
    if (!p2) { buildByID(&result, p1, &status); return result; }
    buildDifference(&result, p1, p2, &status);
    return result;
}

SongNode *symmetricDifferenceOfPlaylist(SongNode *p1, SongNode *p2)
{
    SongNode *diff1   = differenceOfPlaylist(p1, p2);
    SongNode *diff2   = differenceOfPlaylist(p2, p1);
    SongNode *result  = unionOfPlaylist(diff1, diff2);
    freeTree(diff1);
    freeTree(diff2);
    return result;
}

// ─────────────────────────────────────────────
//  History (doubly linked list) — UNCHANGED
// ─────────────────────────────────────────────

void addToHistory(HistoryNode **tail, HistoryNode **head, SongNode *song)
{
    // song is the temporary single-key node returned by searchByID
    HistoryNode *newNode = (HistoryNode *)malloc(sizeof(HistoryNode));
    newNode->songID = song->keys[0].songID;
    strcpy(newNode->title,  song->keys[0].title);
    strcpy(newNode->artist, song->keys[0].artist);

    if (!(*head)) *head = newNode;

    newNode->prev = *tail;
    newNode->next = NULL;

    if (*tail) (*tail)->next = newNode;
    *tail = newNode;
}

void playSong(SongNode *playlist, HistoryNode **tail, HistoryNode **head, int id)
{
    SongNode *song = searchByID(playlist, id);
    if (!song) {
        printf("\nSong with ID: %d not found in the Playlist!\n", id);
    } else {
        printf("\nPlaying Song: %s.....\n", song->keys[0].title);
        addToHistory(tail, head, song);
        free(song);  // Free the temporary node returned by searchByID
    }
}

void displayHistoryChronologically(HistoryNode *head)
{
    HistoryNode *temp = head;
    printf("\nHistory: \n");
    while (temp) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ",
               temp->songID, temp->title, temp->artist);
        temp = temp->next;
    }
    printf("NULL\n");
}

void displayHistoryReverseChronologically(HistoryNode *tail)
{
    HistoryNode *temp = tail;
    printf("\nHistory: \n");
    while (temp) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ",
               temp->songID, temp->title, temp->artist);
        temp = temp->prev;
    }
    printf("NULL\n");
}

// ─────────────────────────────────────────────
//  playPlaylist: traverse tree inorder and add each song to history
// ─────────────────────────────────────────────

void playPlaylist(SongNode *node, HistoryNode **tail, HistoryNode **head)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            playPlaylist(node->children[i], tail, head);
        printf("[ID: %d, Title: %s, Artist: %s] -> ",
               node->keys[i].songID, node->keys[i].title, node->keys[i].artist);

        // Create a temporary node to pass into addToHistory
        SongNode tmp;
        tmp.keys[0] = node->keys[i];
        tmp.n = 1;
        tmp.leaf = true;
        addToHistory(tail, head, &tmp);
    }
    if (!node->leaf)
        playPlaylist(node->children[i], tail, head);
}

// ─────────────────────────────────────────────
//  File save / load
// ─────────────────────────────────────────────

// Iterative inorder traversal using an explicit stack to save songs to file
void savePlaylistsToFile(SongNode *playlists[], int count)
{
    FILE *fp = fopen("playlists.txt", "w");
    if (!fp) { printf("Error opening file for writing.\n"); return; }

    fprintf(fp, "%d\n", count);

    for (int i = 0; i < count; i++) {
        SongNode *node = playlists[i];

        // We push nodes and track which key index we're on
        typedef struct { SongNode *nd; int ki; } Frame;
        Frame frames[5000];
        int   ftop = -1;

        // Start: push root with ki=0 and go left
        if (node) {
            frames[++ftop] = (Frame){node, 0};
            while (!node->leaf) {
                node = node->children[0];
                frames[++ftop] = (Frame){node, 0};
            }
        }

        while (ftop >= 0) {
            Frame *f = &frames[ftop];
            if (f->ki < f->nd->n) {
                // Print keys[f->ki]
                fprintf(fp, "%d %d %s %s\n",
                        i,
                        f->nd->keys[f->ki].songID,
                        f->nd->keys[f->ki].title,
                        f->nd->keys[f->ki].artist);
                f->ki++;

                // Descend into children[f->ki] (which is now ki, post-increment)
                if (!f->nd->leaf) {
                    SongNode *child = f->nd->children[f->ki];
                    frames[++ftop] = (Frame){child, 0};
                    while (!child->leaf) {
                        child = child->children[0];
                        frames[++ftop] = (Frame){child, 0};
                    }
                }
            } else {
                ftop--;  // Done with this node
            }
        }
    }

    fclose(fp);
    printf("\nPlaylists saved successfully!\n");
}

void loadPlaylistsFromFile(SongNode *playlists[], int *count)
{
    FILE *fp = fopen("playlists.txt", "r");
    if (!fp) { printf("No previous data found.\n"); return; }

    fscanf(fp, "%d", count);
    for (int i = 0; i < *count; i++)
        playlists[i] = NULL;

    int  pIndex, id;
    char title[100], artist[100];
    while (fscanf(fp, "%d %d %s %s", &pIndex, &id, title, artist) != EOF) {
        int status = 0;
        playlists[pIndex] = addSong(playlists[pIndex], id, title, artist, &status);
    }

    fclose(fp);
    printf("\nPlaylists loaded successfully!\n");
}

// ─────────────────────────────────────────────
//  Menu helper functions (identical logic to original)
// ─────────────────────────────────────────────

void selectPlaylist(int count)
{
    if (count == 0)
        printf("\nNo Playlist created.\n");
    else {
        printf("\nSelect a Playlist:\n");
        for (int i = 1; i <= count; i++)
            printf("\n%d.Playlist %d", i, i);
    }
}

int choosePlaylist(SongNode *playlists[], int count)
{
    selectPlaylist(count);
    int playlistNo = 0;
    if (count > 0) {
        printf("\n\nEnter your choice: ");
        scanf("%d", &playlistNo);
    }
    if (playlistNo > count) {
        printf("\nInvalid choice.\n");
        playlistNo = 0;
    }
    return playlistNo;
}

void displaySongs(SongNode *root)
{
    printf("\n");
    inorder(root);
    printf("NULL\n");
}

void handleAddSong(SongNode *playlists[], int *count)
{
    int choose, id;
    char title[100], artist[100];
    selectPlaylist(*count);
    if (*count < 10)
        printf("\n\n%d.Create new Playlist.\n", (*count) + 1);
    printf("\nEnter your choice: ");
    scanf("%d", &choose);

    if (choose > 0 && choose <= (*count) + 1) {
        if (choose == (*count) + 1) {
            playlists[choose - 1] = NULL;
            (*count)++;
        }
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        printf("Enter the Title: ");
        scanf("%s", title);
        printf("Enter the Artist: ");
        scanf("%s", artist);

        if (id > 0) {
            int status = 0;
            playlists[choose - 1] = addSong(playlists[choose - 1], id, title, artist, &status);
            if (status)
                printf("\nSong added successfully!\n");
            else
                printf("\nSong with ID: %d already exists!\n", id);
        } else {
            printf("\nSong ID should be greater than 0.\n");
        }
    } else {
        printf("\nInvalid choice.\n");
    }
}

void handleDeleteSong(SongNode *playlists[], int count)
{
    int choice = choosePlaylist(playlists, count);
    if (choice) {
        int id, status = 0;
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        playlists[choice - 1] = deleteSong(playlists[choice - 1], id, &status);
        if (status)
            printf("\nSong deleted successfully!\n");
        else
            printf("\nSong with ID: %d not found!\n", id);
    }
}

void handleDisplay(SongNode *playlists[], int count)
{
    int choice = choosePlaylist(playlists, count);
    if (choice) {
        printf("\n1.Display by Title.\n2.Display by Artist.\n3.Display by Artist and Title.\n");
        int select;
        printf("\nEnter your choice: ");
        scanf("%d", &select);
        SongNode *root = NULL;
        if (select == 1) {
            buildByTitle(&root, playlists[choice - 1]);
            displaySongs(root);
            freeTree(root);
        } else if (select == 2) {
            buildByArtist(&root, playlists[choice - 1]);
            displaySongs(root);
            freeTree(root);
        } else if (select == 3) {
            buildByArtistAndTitle(&root, playlists[choice - 1]);
            displaySongs(root);
            freeTree(root);
        } else {
            printf("\nInvalid choice.\n");
        }
    }
}

void handleSearch(SongNode *playlists[], int count)
{
    int choice = choosePlaylist(playlists, count);
    if (choice) {
        printf("\n1.Search by ID.\n2.Search by Title.\n3.Search by Artist.\n");
        int select;
        printf("\nEnter your choice: ");
        scanf("%d", &select);
        if (select == 1) {
            int id;
            printf("\nEnter the Song ID: ");
            scanf("%d", &id);
            SongNode *song = searchByID(playlists[choice - 1], id);
            if (!song) {
                printf("\nSong not found in the Playlist.\n");
            } else {
                printf("\n[ID: %d, Title: %s, Artist: %s]\n",
                       song->keys[0].songID, song->keys[0].title, song->keys[0].artist);
                free(song);
            }
        } else if (select == 2) {
            char title[100];
            printf("\nEnter the Song Title: ");
            scanf("%s", title);
            printf("\n");
            searchByTitle(playlists[choice - 1], title);
            printf("NULL\n");
        } else if (select == 3) {
            char artist[100];
            printf("\nEnter the Song Artist: ");
            scanf("%s", artist);
            printf("\n");
            searchByArtist(playlists[choice - 1], artist);
            printf("NULL\n");
        } else {
            printf("\nInvalid choice.\n");
        }
    }
}

void handleShuffle(SongNode *playlists[], int count)
{
    int choice = choosePlaylist(playlists, count);
    if (choice)
        shufflePlaylist(playlists[choice - 1]);
}

void handleRangeSearch(SongNode *playlists[], int count)
{
    int choice = choosePlaylist(playlists, count);
    if (choice) {
        int id1, id2;
        printf("\nEnter Song ID 1: ");
        scanf("%d", &id1);
        printf("\nEnter Song ID 2: ");
        scanf("%d", &id2);
        if (id1 <= id2) {
            printf("\n");
            rangeSearch(playlists[choice - 1], id1, id2);
            printf("NULL\n");
        } else {
            printf("\nSong ID 2 should be greater than Song ID 1.\n");
        }
    }
}

void handlePlaySong(SongNode *playlists[], int count, HistoryNode **tail, HistoryNode **head)
{
    int choice = choosePlaylist(playlists, count);
    if (choice) {
        int id;
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        playSong(playlists[choice - 1], tail, head, id);
    }
}

void handlePlayPlaylist(SongNode *playlists[], int count, HistoryNode **tail, HistoryNode **head)
{
    int choice = choosePlaylist(playlists, count);
    if (choice) {
        printf("\n");
        playPlaylist(playlists[choice - 1], tail, head);
        printf("NULL\n");
    }
}

void handleRepeat(SongNode *playlists[], int count, HistoryNode **tail, HistoryNode **head)
{
    int choice = choosePlaylist(playlists, count);
    if (choice) {
        int flag = 1, r = 0;
        while (flag) {
            printf("\n");
            playPlaylist(playlists[choice - 1], tail, head);
            printf("NULL\n");
            r++;
            if (r > 5) {
                printf("\nDo you want to repeat again ?\n1.Yes\n2. No.\n");
                int select;
                printf("\nEnter your choice: ");
                scanf("%d", &select);
                if (select == 2)
                    flag = 0;
                else
                    r = 0;
            }
        }
    }
}

void handleHistory(SongNode *playlists[], int count, HistoryNode *tail, HistoryNode *head)
{
    (void)playlists; (void)count;
    printf("\n1.Display history chronologically.\n2.Display history reverse chronologically.\n");
    int select;
    printf("\nEnter your choice: ");
    scanf("%d", &select);
    if (select == 1)
        displayHistoryChronologically(head);
    else if (select == 2)
        displayHistoryReverseChronologically(tail);
    else
        printf("\nInvalid choice.\n");
}

void freePlaylists(SongNode *playlists[], int count)
{
    for (int i = 0; i < count; i++) {
        freeTree(playlists[i]);
        playlists[i] = NULL;
    }
}

void handleMultiplePlaylists(SongNode *playlists[], int *count)
{
    selectPlaylist(*count);
    if (*count >= 2) {
        int choice1, choice2;
        printf("\n\nEnter Playlist 1: ");
        scanf("%d", &choice1);
        if (choice1 <= *count && choice1 != 0) {
            printf("Enter Playlist 2: ");
            scanf("%d", &choice2);
            if (choice2 <= *count && choice2 != 0) {
                if (choice1 != choice2) {
                    if (*count != 10) {
                        printf("\nChoose an operation:\n");
                        printf("\n1.Union.\n2.Intersection.\n3.Difference.\n4.Symmetric Difference.\n");
                        int choice;
                        printf("\nEnter your choice: ");
                        scanf("%d", &choice);
                        switch (choice) {
                        case 1:
                            playlists[*count] = unionOfPlaylist(playlists[choice1 - 1], playlists[choice2 - 1]);
                            printf("\nUnion done successfully!\n\nUnion:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;
                        case 2:
                            playlists[*count] = intersectionOfPlaylist(playlists[choice1 - 1], playlists[choice2 - 1]);
                            printf("\nIntersection done successfully!\n\nIntersection:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;
                        case 3:
                            playlists[*count] = differenceOfPlaylist(playlists[choice1 - 1], playlists[choice2 - 1]);
                            printf("\nDifference done successfully!\n\nDifference:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;
                        case 4:
                            playlists[*count] = symmetricDifferenceOfPlaylist(playlists[choice1 - 1], playlists[choice2 - 1]);
                            printf("\nSymmetric Difference done successfully!\n\nSymmetric Difference:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;
                        default:
                            printf("\nInvalid choice.\n");
                            break;
                        }
                    } else {
                        printf("\nCannot create more than 10 Playlists.\n");
                    }
                } else {
                    printf("\nPlaylist 1 and Playlist 2 should not be same.\n");
                }
            } else {
                printf("\nInvalid choice.\n");
            }
        } else {
            printf("\nInvalid choice.\n");
        }
    } else if (*count != 0) {
        printf("\nCreate atleast 2 Playlists to perform operations.\n");
    }
}

// ─────────────────────────────────────────────
//  main
// ─────────────────────────────────────────────

int main()
{
    SongNode *playlists[10];
    for (int i = 0; i < 10; i++)
        playlists[i] = NULL;

    HistoryNode *historyHead = NULL;
    HistoryNode *historyTail = NULL;
    int done = 0, playlistCount = 0;

    loadPlaylistsFromFile(playlists, &playlistCount);

    while (!done) {
        printf("\n1.Add a song.\n2.Delete a song.\n3.Display Playlist.\n4.Search a song.\n"
               "5.Shuffle Mode.\n6.Repeat Mode.\n7.Range Search.\n8.Play a Playlist.\n"
               "9.Play a song.\n10.Display History.\n11.Operations on multiple playlists.\n12.Exit.\n");

        int choice;
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:  handleAddSong(playlists, &playlistCount);                              break;
        case 2:  handleDeleteSong(playlists, playlistCount);                            break;
        case 3:  handleDisplay(playlists, playlistCount);                               break;
        case 4:  handleSearch(playlists, playlistCount);                                break;
        case 5:  handleShuffle(playlists, playlistCount);                               break;
        case 6:  handleRepeat(playlists, playlistCount, &historyTail, &historyHead);    break;
        case 7:  handleRangeSearch(playlists, playlistCount);                           break;
        case 8:  handlePlayPlaylist(playlists, playlistCount, &historyTail, &historyHead); break;
        case 9:  handlePlaySong(playlists, playlistCount, &historyTail, &historyHead);  break;
        case 10: handleHistory(playlists, playlistCount, historyTail, historyHead);     break;
        case 11: handleMultiplePlaylists(playlists, &playlistCount);                    break;
        case 12:
            savePlaylistsToFile(playlists, playlistCount);
            freePlaylists(playlists, playlistCount);
            {
                HistoryNode *temp = historyHead;
                while (temp) {
                    HistoryNode *next = temp->next;
                    free(temp);
                    temp = next;
                }
                historyTail = NULL;
            }
            done = 1;
            break;
        default:
            printf("\nInvalid choice.\n");
            break;
        }
    }

    return 0;
}