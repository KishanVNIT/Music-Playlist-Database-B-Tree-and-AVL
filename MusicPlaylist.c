#include <stdio.h>
#include <stdlib.h>
#include<string.h>
typedef struct SongNode {
    int songID;
    char title[100];
    char artist[100];
    struct SongNode* left;
    struct SongNode* right;
    int height;
}SongNode;

typedef struct HistoryNode {
    int songID;
    char title[100];
    char artist[100];
    struct HistoryNode* prev;
    struct HistoryNode* next;
}HistoryNode;


int height(SongNode* song) {
    int height = 0;
    if(song) height = song->height;
    return height;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

SongNode* createSong(int id, char title[], char artist[]) {
    SongNode* newSong = (SongNode*)malloc(sizeof(SongNode));
    newSong->songID = id;
    strcpy(newSong->title, title);
    strcpy(newSong->artist, artist);
    newSong->left = newSong->right = NULL;
    newSong->height = 1;
    return newSong;
}

SongNode* rotateRight(SongNode* root) {
    SongNode* l = root->left;
    SongNode* t2 = l->right;

    l->right = root;
    root->left = t2;

    root->height = max(height(root->left), height(root->right)) + 1;
    l->height = max(height(l->left), height(l->right)) + 1;

    return l;
}

SongNode* rotateLeft(SongNode* root) {
    SongNode* r = root->right;
    SongNode* t2 = r->left;

    r->left = root;
    root->right = t2;

    root->height = max(height(root->left), height(root->right)) + 1;
    r->height = max(height(r->left), height(r->right)) + 1;

    return r;
}

int getBalance(SongNode* song) {
    if (song == NULL) return 0;
    return height(song->left) - height(song->right);
}

void freeTree(SongNode* playlist) {
    if(!playlist) return;

    freeTree(playlist->left);
    freeTree(playlist->right);
    free(playlist);
}

void selectPlaylist(int count) {
    if(count == 0) {
        printf("\nNo Playlist created.\n");
    }
    else {
        printf("\nSelect a Playlist:\n");
        int i = 1;
        while(i <= count) {
            printf("\n%d.Playlist %d", i, i);
            i++;
        }
    }
}

int choosePlaylist(SongNode* playlists[], int count) {
    selectPlaylist(count);
    int playlistNo = 0;
    if(count > 0) {
        printf("\n\nEnter your choice: ");
        scanf("%d", &playlistNo);
    }

    if(playlistNo > count) {
        printf("\nInvalid choice.\n");
        playlistNo = 0;
    }

    return playlistNo;
}

void inorder(SongNode* playlist) {

    if (playlist) {
        inorder(playlist->left);
        printf("[ID: %d, Title: %s, Artist: %s] -> ", playlist->songID, playlist->title, playlist->artist);
        inorder(playlist->right);
    }
}

SongNode* addSong(SongNode* playlist, int id, char t[], char a[], int* status) {
    
    if (playlist == NULL) {
        SongNode* newSong = createSong(id, t, a);
        *status = 1;
        return newSong;
    }

    if (id < playlist->songID) {
        playlist->left = addSong(playlist->left, id, t, a, status);
    }

    else if (id > playlist->songID) {
        playlist->right = addSong(playlist->right, id, t, a, status);
    }

    else {
        *status = 0;
        return playlist;
    }

    playlist->height = 1 + max(height(playlist->left), height(playlist->right));

    int balance = getBalance(playlist);

    // LL Case
    if (balance > 1 && getBalance(playlist->left) >= 0)
        playlist = rotateRight(playlist);

    // RR Case
    if (balance < -1 && getBalance(playlist->right) <= 0)
        playlist = rotateLeft(playlist);

    // LR Case
    if (balance > 1 && getBalance(playlist->left) < 0) {
        playlist->left = rotateLeft(playlist->left);
        playlist = rotateRight(playlist);
    }

    // RL Case
    if (balance < -1 && getBalance(playlist->right) > 0) {
        playlist->right = rotateRight(playlist->right);
        playlist = rotateLeft(playlist);
    }


    return playlist;
}

void handleAddSong(SongNode* playlists[], int* count) {
    int choose;
    int id;
    char title[100], artist[100];
    selectPlaylist(*count);
    if(*count < 10) {
        printf("\n\n%d.Create new Playlist.\n", (*count)+1);
    }
    printf("\nEnter your choice: ");
    scanf("%d", &choose);
    if(choose > 0 && choose <= (*count)+1) {
        if(choose == (*count)+1) {
            playlists[choose-1] = NULL;
            (*count)++;
        }
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        printf("Enter the Title: ");
        scanf("%s", title);
        printf("Enter the Artist: ");
        scanf("%s", artist);

        if(id > 0) {
            int status = 0;
            playlists[choose-1] = addSong(playlists[choose-1], id, title, artist, &status);
            if(status) {
                printf("\nSong added successfully!\n");
            }
            else {
                printf("\nSong with ID: %d already exists!\n", id);
            }
        }

        else {
            printf("\nSong ID should be greater than 0.\n");
        }
        
    }
    
    else {
        printf("\nInvalid choice.\n");
    }

            
}

void buildByID(SongNode** newPlaylist, SongNode* oldPlaylist, int* status) {
    if(!oldPlaylist) return;

    buildByID(newPlaylist, oldPlaylist->left, status);
    *newPlaylist = addSong(*newPlaylist, oldPlaylist->songID, oldPlaylist->title, oldPlaylist->artist, status); 
    buildByID(newPlaylist, oldPlaylist->right, status);
}

SongNode* getSuccessor(SongNode* root, SongNode** successor) {
    if (root->left == NULL) {
        *successor = root;
        return root->right;
    }

    root->left = getSuccessor(root->left, successor);

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance < -1) {
        if (getBalance(root->right) <= 0)
            return rotateLeft(root);
        else {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
    }

    return root;
}

SongNode* deleteSong(SongNode* playlist, int id, int* status) {
    if(id <= 0) return playlist;

    if (playlist == NULL) {
        return NULL;
    }

    if (id < playlist->songID) {
        playlist->left = deleteSong(playlist->left, id, status);
    }
    else if (id > playlist->songID) {
        playlist->right = deleteSong(playlist->right, id, status);
    }
    else {
        if (playlist->left == NULL) {
            SongNode* temp = playlist->right;
            free(playlist);
            *status = 1;
            return temp;
        }
        if (playlist->right == NULL) {
            SongNode* temp = playlist->left;
            free(playlist);
            *status = 1;
            return temp;
        }

        SongNode* successor = NULL;

        playlist->right = getSuccessor(playlist->right, &successor);

        successor->left = playlist->left;
        successor->right = playlist->right;

        free(playlist);
        playlist = successor;

        *status = 1;
    }

    playlist->height = 1 + max(height(playlist->left), height(playlist->right));

    int balance = getBalance(playlist);

    // LL
    if (balance > 1 && getBalance(playlist->left) >= 0)
        return rotateRight(playlist);

    // LR
    if (balance > 1 && getBalance(playlist->left) < 0) {
        playlist->left = rotateLeft(playlist->left);
        return rotateRight(playlist);
    }

    // RR
    if(balance < -1 && getBalance(playlist->right) <= 0)
        return rotateLeft(playlist);

    // RL
    if(balance < -1 && getBalance(playlist->right) > 0) {
        playlist->right = rotateRight(playlist->right);
        return rotateLeft(playlist);
    }

    return playlist;
}

void handleDeleteSong(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    int id;
    int status = 0;
    if(choice) {
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        playlists[choice-1] = deleteSong(playlists[choice-1], id, &status);
        
        if(status) {
            printf("\nSong deleted successfully!\n");
        }
        else {
            printf("\nSong with ID: %d not found!\n", id);
        }
    }
}

SongNode* copyByTitle(SongNode* playlist, int id, char t[], char a[]) {
    
    if(playlist == NULL) {
        SongNode* newSong = createSong(id, t, a);
        return newSong;
    }

    if(strcmp(t, playlist->title) < 0) {
        playlist->left = copyByTitle(playlist->left, id, t, a);
    }

    else if(strcmp(t, playlist->title) > 0) {
        playlist->right = copyByTitle(playlist->right, id, t, a);
    }

    else {
        if(id < playlist->songID) {
            playlist->left = copyByTitle(playlist->left, id, t, a);
        }
        else {
            playlist->right = copyByTitle(playlist->right, id, t, a);
        }
    }

    playlist->height = 1 + max(height(playlist->left), height(playlist->right));

    int balance = getBalance(playlist);

    // LL
    if (balance > 1 && getBalance(playlist->left) >= 0)
        playlist = rotateRight(playlist);

    // RR
    if (balance < -1 && getBalance(playlist->right) <= 0)
        playlist = rotateLeft(playlist);

    // LR
    if (balance > 1 && getBalance(playlist->left) < 0) {
        playlist->left = rotateLeft(playlist->left);
        playlist = rotateRight(playlist);
    }

    // RL
    if (balance < -1 && getBalance(playlist->right) > 0) {
        playlist->right = rotateRight(playlist->right);
        playlist = rotateLeft(playlist);
    }


    return playlist;
}

void buildByTitle(SongNode** titleRoot, SongNode* idRoot) {
    if(!idRoot) return;

    buildByTitle(titleRoot, idRoot->left);
    *titleRoot = copyByTitle(*titleRoot, idRoot->songID, idRoot->title, idRoot->artist);
    buildByTitle(titleRoot, idRoot->right);    
}

SongNode* copyByArtist(SongNode* playlist, int id, char t[], char a[]) {
    
    if(playlist == NULL) {
        SongNode* newSong = createSong(id, t, a);
        return newSong;
    }

    if(strcmp(a, playlist->artist) < 0) {
        playlist->left = copyByArtist(playlist->left, id, t, a);
    }

    else if(strcmp(a, playlist->artist) > 0) {
        playlist->right = copyByArtist(playlist->right, id, t, a);
    }

    else {
        if(id < playlist->songID) {
            playlist->left = copyByArtist(playlist->left, id, t, a);
        }
        else {
            playlist->right = copyByArtist(playlist->right, id, t, a);
        }
    }

    playlist->height = 1 + max(height(playlist->left), height(playlist->right));

    int balance = getBalance(playlist);

    // LL
    if (balance > 1 && getBalance(playlist->left) >= 0)
        playlist = rotateRight(playlist);

    // RR
    if (balance < -1 && getBalance(playlist->right) <= 0)
        playlist = rotateLeft(playlist);

    // LR
    if (balance > 1 && getBalance(playlist->left) < 0) {
        playlist->left = rotateLeft(playlist->left);
        playlist = rotateRight(playlist);
    }

    // RL
    if (balance < -1 && getBalance(playlist->right) > 0) {
        playlist->right = rotateRight(playlist->right);
        playlist = rotateLeft(playlist);
    }


    return playlist;
}

void buildByArtist(SongNode** artistRoot, SongNode* idRoot) {
    if(!idRoot) return;

    buildByArtist(artistRoot, idRoot->left);
    *artistRoot = copyByArtist(*artistRoot, idRoot->songID, idRoot->title, idRoot->artist);
    buildByArtist(artistRoot, idRoot->right);    
}

SongNode* copyByArtistAndTitle(SongNode* playlist, int id, char t[], char a[]) {
    
    if(playlist == NULL) {
        SongNode* newSong = createSong(id, t, a);
        return newSong;
    }

    if(strcmp(a, playlist->artist) < 0) {
        playlist->left = copyByArtistAndTitle(playlist->left, id, t, a);
    }

    else if(strcmp(a, playlist->artist) > 0) {
        playlist->right = copyByArtistAndTitle(playlist->right, id, t, a);
    }

    else {
        if(strcmp(t, playlist->title) < 0) {
            playlist->left = copyByArtistAndTitle(playlist->left, id, t, a);
        }
        else {
            playlist->right = copyByArtistAndTitle(playlist->right, id, t, a);
        }
    }

    playlist->height = 1 + max(height(playlist->left), height(playlist->right));

    int balance = getBalance(playlist);

    // LL
    if (balance > 1 && getBalance(playlist->left) >= 0)
        playlist = rotateRight(playlist);

    // RR
    if (balance < -1 && getBalance(playlist->right) <= 0)
        playlist = rotateLeft(playlist);

    // LR
    if (balance > 1 && getBalance(playlist->left) < 0) {
        playlist->left = rotateLeft(playlist->left);
        playlist = rotateRight(playlist);
    }

    // RL
    if (balance < -1 && getBalance(playlist->right) > 0) {
        playlist->right = rotateRight(playlist->right);
        playlist = rotateLeft(playlist);
    }


    return playlist;
}

void buildByArtistAndTitle(SongNode** artistTitleRoot, SongNode* idRoot) {
    if(!idRoot) return;

    buildByArtistAndTitle(artistTitleRoot, idRoot->left);
    *artistTitleRoot = copyByArtistAndTitle(*artistTitleRoot, idRoot->songID, idRoot->title, idRoot->artist);
    buildByArtistAndTitle(artistTitleRoot, idRoot->right);    
}

void displaySongs(SongNode* playlist) {
    printf("\n");
    inorder(playlist);
    printf("NULL\n");
}

void handleDisplay(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        printf("\n1.Display by Title.\n2.Display by Artist.\n3.Display by Artist and Title.\n");
        int select;
        printf("\nEnter your choice: ");
        scanf("%d", &select);
        SongNode* root = NULL;
        if(select == 1) {
            buildByTitle(&root, playlists[choice-1]);
            displaySongs(root);
            freeTree(root);
        }
        else if(select == 2) {
            buildByArtist(&root, playlists[choice-1]);
            displaySongs(root);
            freeTree(root);
        }
        else if(select == 3) {
            buildByArtistAndTitle(&root, playlists[choice-1]);
            displaySongs(root);
            freeTree(root);
        }
        else printf("\nInvalid choice.\n");
    }
}

SongNode* searchByID(SongNode* playlist, int id) {
    if(id <= 0) return NULL;
    if(!playlist) return NULL;
    if(id == playlist->songID) return playlist;
    if(id < playlist->songID) return searchByID(playlist->left, id);
    return searchByID(playlist->right, id);
}

void searchByTitle(SongNode* playlist, char title[]) {
    if(!playlist) return;
    searchByTitle(playlist->left, title);
    if(strcmp(title, playlist->title) == 0) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", playlist->songID, playlist->title, playlist->artist);
    }
    searchByTitle(playlist->right, title);
}

void searchByArtist(SongNode* playlist, char artist[]) {
    if(!playlist) return;
    searchByArtist(playlist->left, artist);
    if(strcmp(artist, playlist->artist) == 0) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", playlist->songID, playlist->title, playlist->artist);
    }
    searchByArtist(playlist->right, artist);
}

void handleSearch(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        printf("\n1.Search by ID.\n2.Search by Title.\n3.Search by Artist.\n");
        int select;
        printf("\nEnter your choice: ");
        scanf("%d", &select);
        if(select == 1) {
            int id;
            printf("\nEnter the Song ID: ");
            scanf("%d", &id);
            
            SongNode* song = searchByID(playlists[choice-1], id);
            if(!song) {
                printf("\nSong not found in the Playlist.\n");
            }
            else {
                printf("\n[ID: %d, Title: %s, Artist: %s]\n", song->songID, song->title, song->artist);
            }
        }
        else if(select == 2) {
            char title[100];
            printf("\nEnter the Song Title: ");
            scanf("%s", title);
            printf("\n");
            searchByTitle(playlists[choice-1], title);
            printf("NULL\n");
        }
        else if(select == 3) {
            char artist[100];
            printf("\nEnter the Song Artist: ");
            scanf("%s", artist);
            printf("\n");
            searchByArtist(playlists[choice-1], artist);
            printf("NULL\n");
        }
        else {
            printf("\nInvalid choice.\n");
        }
    }
}

void countSongs(SongNode* playlist, int* count) {
    if(playlist) {
        countSongs(playlist->left, count);
        (*count)++;
        countSongs(playlist->right, count);
    }
} 

void storeSongs(SongNode* playlist, SongNode* arr[], int* idx) {
    if(playlist) {
        storeSongs(playlist->left, arr, idx);
        arr[*idx] = playlist;
        (*idx)++;
        storeSongs(playlist->right, arr, idx);
    }
}

void shufflePlaylist(SongNode* playlist) {
    int n = 0;
    countSongs(playlist, &n);
    SongNode* arr[n];
    int start = 0;
    storeSongs(playlist, arr, &start);

    for(int i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        SongNode* t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }     //Fisher Yates

    printf("\n");
    for(int i=0;i<n;i++) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", arr[i]->songID, arr[i]->title, arr[i]->artist);
    }
    printf("NULL\n");

}

void handleShuffle(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        shufflePlaylist(playlists[choice-1]);
    }
}

void rangeSearch(SongNode* playlist, int s1, int s2) {
    if(!playlist) return;

    if(s1 < playlist->songID) rangeSearch(playlist->left, s1, s2);

    if(s1 <= playlist->songID && s2 >= playlist->songID) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", playlist->songID, playlist->title, playlist->artist);
    }

    if(s2 > playlist->songID) rangeSearch(playlist->right, s1, s2);
}

void handleRangeSearch(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        int id1, id2;
        printf("\nEnter Song ID 1: ");
        scanf("%d", &id1);
        printf("\nEnter Song ID 2: ");
        scanf("%d", &id2);
        if(id1 <= id2) {
            printf("\n");
            rangeSearch(playlists[choice-1], id1, id2);
            printf("NULL\n");
        }
        else {
            printf("\nSong ID 2 should be greater than Song ID 1.\n");
        }
    }
}

void addToHistory(HistoryNode** historyTail, HistoryNode** historyHead, SongNode* song) {
    HistoryNode* newTail = (HistoryNode*)malloc(sizeof(HistoryNode));
    newTail->songID = song->songID;
    strcpy(newTail->title, song->title);
    strcpy(newTail->artist, song->artist);

    if(!(*historyHead)) *historyHead = newTail;

    newTail->prev = *historyTail;
    newTail->next = NULL;

    if(*historyTail) (*historyTail)->next = newTail;

    *historyTail = newTail;
}

void playSong(SongNode* playlist, HistoryNode** tail, HistoryNode** head, int id) {
    SongNode* song = searchByID(playlist, id);
    if(!song) {
        printf("\nSong with ID: %d not found in the Playlist!\n", id);
    }
    else {
        printf("\nPlaying Song: %s.....\n", song->title);
        addToHistory(tail, head, song);
    }
}

void handlePlaySong(SongNode* playlists[], int count, HistoryNode** tail, HistoryNode** head) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        int id;
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        playSong(playlists[choice-1], tail, head, id);
    }
}

void displayHistoryChronologically(HistoryNode* head) {
    HistoryNode* temp = head;
    printf("\nHistory: \n");
    while(temp) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", temp->songID, temp->title, temp->artist);

        temp = temp->next;
    }

    printf("NULL\n");
}

void displayHistoryReverseChronologically(HistoryNode* tail) {
    HistoryNode* temp = tail;
    printf("\nHistory: \n");
    while(temp) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", temp->songID, temp->title, temp->artist);

        temp = temp->prev;
    }

    printf("NULL\n");
}

void handleHistory(SongNode* playlists[], int count, HistoryNode* tail, HistoryNode* head) {
    printf("\n1.Display history chronologically.\n2.Display history reverse chronologically.\n");
    int select;
    printf("\nEnter your choice: ");
    scanf("%d", &select);
    if(select == 1) {
        displayHistoryChronologically(head);
    }
    else if(select == 2) {
        displayHistoryReverseChronologically(tail);
    }
    else {
        printf("\nInvalid choice.\n");
    }
}

void playPlaylist(SongNode* playlist, HistoryNode** tail, HistoryNode** head) {

    if(!playlist) return;
    playPlaylist(playlist->left, tail, head);
    printf("[ID: %d, Title: %s, Artist: %s] -> ", playlist->songID, playlist->title, playlist->artist);
    addToHistory(tail, head, playlist);
    playPlaylist(playlist->right, tail, head);
}

void handlePlayPlaylist(SongNode* playlists[], int count, HistoryNode** tail, HistoryNode** head) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        printf("\n");
        playPlaylist(playlists[choice-1], tail, head);
        printf("NULL\n");
    }
}

void handleRepeat(SongNode* playlists[], int count, HistoryNode** tail, HistoryNode** head) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        int flag = 1, r = 0;
        while(flag) {
            playPlaylist(playlists[choice-1], tail, head);
            r++;
            if(r>5){
                printf("\nDo you want to repeat again ?\n1.Yes\n2. No.\n");
                int select;
                printf("\nEnter your choice: ");
                scanf("%d",&select);
                if(select==2){
                    flag=0;
                }
                else{
                    r=0;
                }

            }
        }
    }
}

void freePlaylists(SongNode* playlists[], int count) {
    for(int i=0;i<count;i++) {
        freeTree(playlists[i]);
        playlists[i] = NULL;
    }
}

void buildUnion(SongNode** playlist1, SongNode* playlist2, int* status) {
    if(!playlist2) return;
    buildUnion(playlist1, playlist2->left, status);
    *playlist1 = addSong(*playlist1, playlist2->songID, playlist2->title, playlist2->artist, status);
    buildUnion(playlist1, playlist2->right, status);
}

SongNode* unionOfPlaylist(SongNode* playlist1, SongNode* playlist2) {
    SongNode* result = NULL;
    int status = 0;
    if(!playlist1) {
        buildByID(&result, playlist2, &status);
        return result;
    }
    if(!playlist2) {
        buildByID(&result, playlist1, &status);
        return result;
    }

    buildByID(&result, playlist1, &status);
    buildUnion(&result, playlist2, &status);

    return result;

}

void buildIntersection(SongNode** result, SongNode* playlist1, SongNode* playlist2, int* status) {
    if(!playlist1) return;

    buildIntersection(result, playlist1->left, playlist2, status);

    if(searchByID(playlist2, playlist1->songID)) {
        *result = addSong(*result, playlist1->songID, playlist1->title, playlist1->artist, status);
    }

    buildIntersection(result, playlist1->right, playlist2, status);
}

SongNode* intersectionOfPlaylist(SongNode* playlist1, SongNode* playlist2) {
    SongNode* result = NULL;
    if(!playlist1 || !playlist2) return result;
    int status = 0;
    buildIntersection(&result, playlist1, playlist2, &status);

    return result;
}

void buildDifference(SongNode** result, SongNode* playlist1, SongNode* playlist2, int* status) {
    if(!playlist1) return;

    buildDifference(result, playlist1->left, playlist2, status);

    if(!searchByID(playlist2, playlist1->songID)) {
        *result = addSong(*result, playlist1->songID, playlist1->title, playlist1->artist, status);
    }

    buildDifference(result, playlist1->right, playlist2, status);
}

SongNode* differenceOfPlaylist(SongNode* playlist1, SongNode* playlist2) {
    SongNode* result = NULL;
    int status = 0;
    if(!playlist1) return result;
    if(!playlist2) {
        buildByID(&result, playlist1, &status);
        return result;
    }

    buildDifference(&result, playlist1, playlist2, &status);

    return result;
}

SongNode* symmetricDifferenceOfPlaylist(SongNode* playlist1, SongNode* playlist2) {
    SongNode* diff1 = differenceOfPlaylist(playlist1, playlist2);
    SongNode* diff2 = differenceOfPlaylist(playlist2, playlist1);

    SongNode* result = unionOfPlaylist(diff1, diff2);

    freeTree(diff1);
    freeTree(diff2);

    return result;
}

void handleMultiplePlaylists(SongNode* playlists[], int* count) {
    selectPlaylist(*count);
    if(*count >= 2) {
        int choice1, choice2;
        printf("\n\nEnter Playlist 1: ");
        scanf("%d", &choice1);
        if(choice1 <= *count && choice1 != 0) {
            printf("Enter Playlist 2: ");
            scanf("%d", &choice2);
            if(choice2 <= *count && choice2 != 0) {
                if(choice1 != choice2) {
                    if(*count != 10) {
                        printf("\nChoose an operation:\n");
                        printf("\n1.Union.\n2.Intersection.\n3.Difference.\n4.Symmetric Difference.\n");
                        int choice;
                        printf("\nEnter your choice: ");
                        scanf("%d", &choice);
                        switch (choice) {
                        case 1:
                            printf("\nUnion done successfully!\n");
                            playlists[*count] = unionOfPlaylist(playlists[choice1-1], playlists[choice2-1]);
                            printf("\nUnion:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;

                        case 2:
                            printf("\nIntersection done successfully!\n");
                            playlists[*count] = intersectionOfPlaylist(playlists[choice1-1], playlists[choice2-1]);
                            printf("\nIntersection:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;

                        case 3:
                            printf("\nDifference done successfully!\n");
                            playlists[*count] = differenceOfPlaylist(playlists[choice1-1], playlists[choice2-1]);
                            printf("\nDifference:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;

                        case 4:
                            printf("\nSymmetric Difference done successfully!\n");
                            playlists[*count] = symmetricDifferenceOfPlaylist(playlists[choice1-1], playlists[choice2-1]);
                            printf("\nSymmetric Difference:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;
                        
                        default:
                            printf("\nInvalid choice.\n");
                            break;
                        }  
                    }
                    else {
                        printf("\nCannot create more than 10 Playlists.\n");
                    }
                }
                else {
                    printf("\nPlaylist 1 and Playlist 2 should not be same,\n");
                }
            }
            else {
                printf("\nInvalid choice.\n");
            }

        }
        else {
            printf("\nInvalid choice.\n");
        }
    }
    else if(*count != 0) {
        printf("\nCreate atleast 2 Playlists to perform operations.\n");
    }
}

void savePlaylistsToFile(SongNode* playlists[], int count) {
    FILE* fp = fopen("playlists.txt", "w");
    if (!fp) {
        printf("Error opening file for writing.\n");
        return;
    }

    fprintf(fp, "%d\n", count);

    for (int i = 0; i < count; i++) {
        SongNode* stack[1000];
        int top = -1;
        SongNode* curr = playlists[i];

        while (curr || top != -1) {
            while (curr) {
                stack[++top] = curr;
                curr = curr->left;
            }

            curr = stack[top--];

            fprintf(fp, "%d %d %s %s\n",
                    i,
                    curr->songID,
                    curr->title,
                    curr->artist);

            curr = curr->right;
        }
    }

    fclose(fp);
    printf("\nPlaylists saved successfully!\n");
}

void loadPlaylistsFromFile(SongNode* playlists[], int* count) {
    FILE* fp = fopen("playlists.txt", "r");
    if (!fp) {
        printf("No previous data found.\n");
        return;
    }

    fscanf(fp, "%d", count);

    for (int i = 0; i < *count; i++) {
        playlists[i] = NULL;
    }

    int pIndex, id;
    char title[100], artist[100];

    while (fscanf(fp, "%d %d %s %s", &pIndex, &id, title, artist) != EOF) {
        int status = 0;
        playlists[pIndex] = addSong(playlists[pIndex], id, title, artist, &status);
    }

    fclose(fp);
    printf("\nPlaylists loaded successfully!\n");
}


int main() {

    
    SongNode* playlists[10];
    for(int i = 0; i<10; i++) {
        playlists[i] = NULL;
    }

    HistoryNode* historyHead = NULL;
    HistoryNode* historyTail = NULL;
    int done = 0;
    int playlistCount = 0;
    loadPlaylistsFromFile(playlists, &playlistCount);

    while(!done) {
        printf("\n1.Add a song.\n2.Delete a song.\n3.Display Playlist.\n4.Search a song.\n5.Shuffle Mode.\n6.Repeat Mode.\n7.Range Search.\n8.Play a Playlist.\n9.Play a song.\n10.Display History.\n11.Operations on multiple playlists.\n12.Exit.\n");

        int choice;
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                handleAddSong(playlists, &playlistCount);
                break;

            case 2:
                handleDeleteSong(playlists, playlistCount);
                break;
        
            case 3:
                handleDisplay(playlists, playlistCount);
                break;

            case 4:
                handleSearch(playlists, playlistCount);
                break;

            case 5:
                handleShuffle(playlists, playlistCount);
                break;

            case 6:
                handleRepeat(playlists, playlistCount, &historyTail, &historyHead);
                break;

            case 7:
                handleRangeSearch(playlists, playlistCount);
                break;

            case 8:
                handlePlayPlaylist(playlists, playlistCount, &historyTail, &historyHead);
                break;

            case 9:
                handlePlaySong(playlists, playlistCount, &historyTail, &historyHead);
                break;

            case 10:
                handleHistory(playlists, playlistCount, historyTail, historyHead);
                break;

            case 11:
                handleMultiplePlaylists(playlists, &playlistCount);
                break;
            
            case 12:
                savePlaylistsToFile(playlists, playlistCount);

                freePlaylists(playlists, playlistCount);

                HistoryNode* temp = historyHead;
                while(temp) {
                    HistoryNode* nextNode = temp->next;
                    free(temp);
                    temp = nextNode;
                }

                historyTail = NULL;
                done = 1;
                break;

            default:
                printf("\nInvalid choice.\n");
                break;
        }

        
    }


    return 0;
}