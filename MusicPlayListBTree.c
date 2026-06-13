#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define T 3  

typedef struct Song {
    int  songID;
    char title[100];
    char artist[100];
} Song;

typedef struct SongNode {
    Song keys[2 * T - 1];      
    struct SongNode *children[2 * T];     
    int n;               
    bool leaf;               
} SongNode;

typedef struct HistoryNode {
    int  songID;
    char title[100];
    char artist[100];
    struct HistoryNode *prev;
    struct HistoryNode *next;
} HistoryNode;

SongNode *createNode(bool leaf)
{
    SongNode *node = (SongNode *)malloc(sizeof(SongNode));
    node->n    = 0;
    node->leaf = leaf;
    for (int i = 0; i < 2 * T; i++)
        node->children[i] = NULL;
    return node;
}
void freeTree(SongNode *root)
{
    if (!root) return;
    if (!root->leaf)
        for (int i = 0; i <= root->n; i++)
            freeTree(root->children[i]);
    free(root);
}

void inorder(SongNode *node)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            inorder(node->children[i]);
        printf("[ID: %d, Title: %s, Artist: %s] -> ",node->keys[i].songID,node->keys[i].title,
            node->keys[i].artist);
    }
    if (!node->leaf)
        inorder(node->children[i]);
}

void splitChild(SongNode *parent, int idx, SongNode *child)
{
    SongNode *newNode = createNode(child->leaf);
    newNode->n = T - 1;

    for (int j = 0; j < T - 1; j++)
        newNode->keys[j] = child->keys[j + T];

    
    if (!child->leaf)
        for (int j = 0; j < T; j++)
            newNode->children[j] = child->children[j + T];

    child->n = T - 1;   

   
    for (int j = parent->n; j >= idx + 1; j--)
        parent->children[j + 1] = parent->children[j];
    parent->children[idx + 1] = newNode;


    for (int j = parent->n - 1; j >= idx; j--)
        parent->keys[j + 1] = parent->keys[j];

  
    parent->keys[idx] = child->keys[T - 1];
    parent->n++;
}


static void insertNonFullByID(SongNode *node, Song song)
{
    int i = node->n - 1;

    if (node->leaf) {
        
        while (i >= 0 && node->keys[i].songID > song.songID)
        {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = song;
        node->n++;
    } else {
      
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


SongNode *searchNodeByID(SongNode *node, int id);

SongNode *addSong(SongNode *root, int id, char t[], char a[], int *status)
{
    if (searchNodeByID(root, id) != NULL) {
        *status = 0;
        return root;
    }

    Song song;
    song.songID = id;
    strncpy(song.title,  t, 99);
     song.title[99]  = '\0';
    strncpy(song.artist, a, 99);
     song.artist[99] = '\0';

    if (root == NULL) {
        root = createNode(true);
        root->keys[0] = song;
        root->n = 1;
        *status = 1;
        return root;
    }

    
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

SongNode *searchNodeByID(SongNode *node, int id)
{
    if (!node) return NULL;

    int i = 0;
    while (i < node->n && id > node->keys[i].songID)
        i++;

    if (i < node->n && node->keys[i].songID == id)
        return node;   

    if (node->leaf)
        return NULL;   

    return searchNodeByID(node->children[i], id);
}

SongNode *searchByID(SongNode *root, int id)
{
    if (id <= 0 || !root) return NULL;

    SongNode *found = searchNodeByID(root, id);
    if (!found) return NULL;

    int i = 0;
    while (i < found->n && found->keys[i].songID != id) i++;

    SongNode *result = createNode(true);
    result->keys[0] = found->keys[i];
    result->n = 1;
    return result;  
}

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

void rangeSearch(SongNode *node, int s1, int s2)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf && s1 < node->keys[i].songID)
            rangeSearch(node->children[i], s1, s2);

        if (node->keys[i].songID >= s1 && node->keys[i].songID <= s2)
            printf("[ID: %d, Title: %s, Artist: %s] -> ",
                   node->keys[i].songID,
                   node->keys[i].title,
                   node->keys[i].artist);

        if (node->keys[i].songID > s2) return;
    }
    if (!node->leaf)
        rangeSearch(node->children[i], s1, s2);
}

 int findKeyIdx(SongNode *node, int id)
{
    int idx = 0;
    while (idx < node->n && node->keys[idx].songID < id)
        idx++;
    return idx;
}

 void removeFromLeaf(SongNode *node, int idx)
{
    for (int i = idx + 1; i < node->n; i++)
        node->keys[i - 1] = node->keys[i];
    node->n--;
}

 Song getPredecessor(SongNode *node, int idx)
{
    SongNode *curr = node->children[idx];
    while (!curr->leaf)
        curr = curr->children[curr->n];
    return curr->keys[curr->n - 1];
}


 Song getSuccessorKey(SongNode *node, int idx)
{
    SongNode *curr = node->children[idx + 1];
    while (!curr->leaf)
        curr = curr->children[0];
    return curr->keys[0];
}

 void mergeChildren(SongNode *node, int idx)
{
    SongNode *left  = node->children[idx];
    SongNode *right = node->children[idx + 1];

    left->keys[left->n] = node->keys[idx];

    if (!left->leaf)
        left->children[left->n + 1] = right->children[0];

    for (int i = 0; i < right->n; i++) {
        left->keys[left->n + 1 + i] = right->keys[i];
        if (!left->leaf)
            left->children[left->n + 2 + i] = right->children[i + 1];
    }

    left->n += right->n + 1;

    for (int i = idx + 1; i < node->n; i++)
        node->keys[i - 1] = node->keys[i];
    for (int i = idx + 2; i <= node->n; i++)
        node->children[i - 1] = node->children[i];

    node->n--;
    free(right);
}

 void borrowFromPrev(SongNode *node, int idx)
{
    SongNode *child   = node->children[idx];
    SongNode *sibling = node->children[idx - 1];

    for (int i = child->n - 1; i >= 0; i--)
        child->keys[i + 1] = child->keys[i];
    if (!child->leaf)
        for (int i = child->n; i >= 0; i--)
            child->children[i + 1] = child->children[i];

    child->keys[0] = node->keys[idx - 1];
    if (!child->leaf)
        child->children[0] = sibling->children[sibling->n];

    node->keys[idx - 1] = sibling->keys[sibling->n - 1];

    child->n++;
    sibling->n--;
}

 void borrowFromNext(SongNode *node, int idx)
{
    SongNode *child   = node->children[idx];
    SongNode *sibling = node->children[idx + 1];

    child->keys[child->n] = node->keys[idx];
    if (!child->leaf)
        child->children[child->n + 1] = sibling->children[0];

    node->keys[idx] = sibling->keys[0];

    for (int i = 1; i < sibling->n; i++)
        sibling->keys[i - 1] = sibling->keys[i];
    if (!sibling->leaf)
        for (int i = 1; i <= sibling->n; i++)
            sibling->children[i - 1] = sibling->children[i];

    child->n++;
    sibling->n--;
}

 void fill(SongNode *node, int idx)
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

 SongNode *deleteHelper(SongNode *node, int id, int *status)
{
    if (!node) return NULL;

    int idx = findKeyIdx(node, id);

    if (idx < node->n && node->keys[idx].songID == id) {
        *status = 1;
        if (node->leaf) {
            removeFromLeaf(node, idx);
        } else {
            if (node->children[idx]->n >= T) {
                Song pred = getPredecessor(node, idx);
                node->keys[idx] = pred;
                node->children[idx] = deleteHelper(node->children[idx], pred.songID, status);
            } else if (node->children[idx + 1]->n >= T) {
                Song succ = getSuccessorKey(node, idx);
                node->keys[idx] = succ;
                node->children[idx + 1] = deleteHelper(node->children[idx + 1], succ.songID, status);
            } else {
                mergeChildren(node, idx);
                node->children[idx] = deleteHelper(node->children[idx], id, status);
            }
        }
    } else {
        if (node->leaf) {
            return node;
        }

        bool isLast = (idx == node->n);

        if (node->children[idx]->n < T)
            fill(node, idx);

        if (isLast && idx > node->n)
            node->children[idx - 1] = deleteHelper(node->children[idx - 1], id, status);
        else
            node->children[idx] = deleteHelper(node->children[idx], id, status);
    }

    return node;
}

SongNode *deleteSong(SongNode *root, int id, int *status)
{
    if (id <= 0 || !root) return root;

    root = deleteHelper(root, id, status);
    if (root && root->n == 0 && !root->leaf) {
        SongNode *oldRoot = root;
        root = root->children[0];
        free(oldRoot);
    }

    return root;
}

 void insertNonFullByTitle(SongNode *node, Song song)
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

 SongNode *addSongByTitle(SongNode *root, Song song)
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



void insertNonFullByArtist(SongNode *node, Song song)
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

 SongNode *addSongByArtist(SongNode *root, Song song)
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

 int cmpArtistTitle(Song *a, Song *b)
{
    int c = strcmp(a->artist, b->artist);
    if (c != 0) return c;
    c = strcmp(a->title, b->title);
    if (c != 0) return c;
    return a->songID - b->songID;
}

 void insertNonFullByArtistAndTitle(SongNode *node, Song song)
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


 SongNode *addSongByArtistAndTitle(SongNode *root, Song song)
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
    if (n == 0) {
         printf("\nPlaylist is empty.\n");
          return;
         }

    Song *arr[n];
    int start = 0;
    storeSongs(root, arr, &start);

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Song *tmp = arr[i]; 
        arr[i] = arr[j];
         arr[j] = tmp;
    }

    printf("\n");
    for (int i = 0; i < n; i++)
        printf("[ID: %d, Title: %s, Artist: %s] -> ",
               arr[i]->songID, arr[i]->title, arr[i]->artist);
    printf("NULL\n");
}

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
    if (!p1) {
         buildByID(&result, p2, &status); 
         return result;
         }
    if (!p2) { 
        buildByID(&result, p1, &status); 
        return result; 
    }
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
    if (!p2) { buildByID(&result, p1, &status);
         return result; 
        }
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

void addToHistory(HistoryNode **tail, HistoryNode **head, SongNode *song)
{
    
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
        free(song);  
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

void playPlaylist(SongNode *node, HistoryNode **tail, HistoryNode **head)
{
    if (!node) return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf)
            playPlaylist(node->children[i], tail, head);
        printf("[ID: %d, Title: %s, Artist: %s] -> ",
               node->keys[i].songID, node->keys[i].title, node->keys[i].artist);

        SongNode tmp;
        tmp.keys[0] = node->keys[i];
        tmp.n = 1;
        tmp.leaf = true;
        addToHistory(tail, head, &tmp);
    }
    if (!node->leaf)
        playPlaylist(node->children[i], tail, head);
}

void savePlaylistsToFile(SongNode *playlists[], int count)
{
    FILE *fp = fopen("playlists.txt", "w");
    if (!fp) { 
        printf("Error opening file for writing.\n");
        return;
         }

    fprintf(fp, "%d\n", count);

    for (int i = 0; i < count; i++) {
        SongNode *node = playlists[i];

    
        typedef struct {
             SongNode *nd;
              int ki;
             } Frame;
        Frame frames[5000];
        int ftop = -1;

      
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
                fprintf(fp, "%d %d %s %s\n",
                        i,
                        f->nd->keys[f->ki].songID,
                        f->nd->keys[f->ki].title,
                        f->nd->keys[f->ki].artist);
                f->ki++;

                if (!f->nd->leaf) {
                    SongNode *child = f->nd->children[f->ki];
                    frames[++ftop] = (Frame){child, 0};
                    while (!child->leaf) {
                        child = child->children[0];
                        frames[++ftop] = (Frame){child, 0};
                    }
                }
            } else {
                ftop--;  
            }
        }
    }

    fclose(fp);
    printf("\nPlaylists saved successfully!\n");
}

void loadPlaylistsFromFile(SongNode *playlists[], int *count)
{
    FILE *fp = fopen("playlists.txt", "r");
    if (!fp) { 
        printf("No previous data found.\n");
        return; 
        }

    fscanf(fp, "%d", count);
    for (int i = 0; i < *count; i++)
        playlists[i] = NULL;

    int  pIndex;
    int id;
    char title[100];
    char artist[100];
    while (fscanf(fp, "%d %d %s %s", &pIndex, &id, title, artist) != EOF) {
        int status = 0;
        playlists[pIndex] = addSong(playlists[pIndex], id, title, artist, &status);
    }

    fclose(fp);
    printf("\nPlaylists loaded successfully!\n");
}

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
    int choose;
    int id;
    char title[100];
    char artist[100];
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
        int id;
        int status = 0;
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
        int id1;
        int id2;
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
        int flag = 1;
        int r = 0;
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

void handleHistory( HistoryNode *tail, HistoryNode *head)
{
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
        int choice1;
        int choice2;
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

int main()
{
    SongNode *playlists[10];
    for (int i = 0; i < 10; i++)
        playlists[i] = NULL;

    HistoryNode *historyHead = NULL;
    HistoryNode *historyTail = NULL;
    int done = 0;
    int playlistCount = 0;

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
        case 10: handleHistory( historyTail, historyHead);     break;
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