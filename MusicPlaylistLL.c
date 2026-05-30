#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef enum {FAILURE, SUCCESS} StatusCode;


typedef struct SongNode {
    int songID;
    char title[100];
    char artist[100];
    struct SongNode* next;
    struct SongNode* prev;
}SongNode;

typedef struct HistoryNode {
    int songID;
    char title[100];
    char artist[100];
    struct HistoryNode* next;
}HistoryNode;

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

StatusCode addSong(SongNode** playlist, int id, char t[], char a[]) {
    StatusCode SC = SUCCESS;
    if(id <= 0) {
        printf("\nSong ID should be greater than 0.\n");
        SC = FAILURE;
    }
    else {
        SongNode* newSong = (SongNode*)malloc(sizeof(SongNode));
        if(!newSong) {
            printf("\nMemory Full! Cannot add more songs.\n");
            SC = FAILURE;
        }
        else {
            newSong->songID = id;
            strcpy(newSong->title, t);
            strcpy(newSong->artist, a);
            newSong->next = newSong->prev = NULL;

            if(*playlist==NULL) *playlist = newSong;  //checking if the playlist is empty
            else {
                SongNode* temp = *playlist, *prevSong = NULL;
                while(temp && temp->songID < id) {
                    prevSong = temp;
                    temp = temp->next;
                } 

                if(temp && temp->songID==id) {
                    printf("\nSong with ID: %d already exists!\n", id);
                    SC = FAILURE;
                    free(newSong);
                }   //checking if song already exists

                else {
                    if(!prevSong) {
                        (*playlist)->prev = newSong;
                        newSong->next = *playlist;
                        *playlist = newSong;
                    }  //checking if the song is first in the playlist
                    else {
                        prevSong->next = newSong;
                        newSong->prev = prevSong;
                        newSong->next = temp;
                        if(temp) temp->prev = newSong;
                    }   //putting the song at its place

                }
            }
        }
    }

    if(SC == SUCCESS) {
        printf("\nSong added successfully!\n");
    }

    return SC;

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
    if(choose>0&&choose <= (*count)+1) {
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
        addSong(&playlists[choose-1], id, title, artist);
    }
    
    else {
        printf("\nInvalid choice.\n");
    }

            
}

SongNode* searchByID(SongNode* playlist, int id) {
    SongNode* temp = playlist;
    SongNode* song = NULL;
    if(id > 0) {
        while(temp && temp->songID < id) {
            temp = temp->next;
        }

        if(temp && temp->songID == id) {
            song = temp;
        }
    }

    return song;
}

StatusCode deleteSong(SongNode** playlist, int id) {
    StatusCode SC = SUCCESS;

    SongNode* temp = searchByID(*playlist, id);
    if(!temp) {
        printf("\nSong with ID: %d not found!\n", id);
        SC = FAILURE;
    }
    else {
        if(temp == *playlist) {
            *playlist = temp->next;
            if(temp->next) temp->next->prev = NULL;
        }   //checking if the song is first in the playlist
        else {
            temp->prev->next = temp->next;
            if(temp->next) temp->next->prev = temp->prev;
        }  //removing the song
        free(temp);
        printf("\nSong deleted successfully!\n");
    }

    return SC;
}

void handleDeleteSong(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    int id;
    if(choice) {
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        deleteSong(&playlists[choice-1], id);
    }
}

SongNode* copyLL(SongNode* head) {
    SongNode* temp = head;
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* prevSong = dummyNode;

    while(temp) {
        SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
        newNode->songID = temp->songID;
        strcpy(newNode->title, temp->title);
        strcpy(newNode->artist, temp->artist);
        prevSong->next = newNode;
        newNode->prev = prevSong;
        newNode->next = NULL;
        prevSong = newNode;
        temp = temp->next;
    }   //copying the LL

    SongNode* copyHead = dummyNode->next;
    if(copyHead) copyHead->prev = NULL;
    free(dummyNode);

    return copyHead;
}

SongNode* splitLL(SongNode* head) {
    SongNode* nextNode = NULL;
    if(head && head->next) {
        SongNode* slow = head, *fast = head;
        while(fast->next && fast->next->next) {
            slow = slow->next;
            fast = fast->next->next;
        }

        nextNode = slow->next;
        slow->next = NULL;
        nextNode->prev = NULL;
    }

    return nextNode;
}

SongNode* mergeLLByTitle(SongNode* head1, SongNode* head2) {
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    SongNode* curr = dummyNode;
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* t1 = head1, *t2 = head2;

    while(t1 && t2) {
        if(strcmp(t1->title, t2->title) < 0) {
            curr->next = t1;
            t1->prev = curr;
            curr = t1;
            t1 = t1->next;
        }
        else {
            curr->next = t2;
            t2->prev = curr;
            curr = t2;
            t2 = t2->next;
        }
    }

    if(t1) {
        curr->next = t1;
        t1->prev = curr;
    }
    else if(t2) {
        curr->next = t2;
        t2->prev = curr;
    }

    SongNode* head = dummyNode->next;
    if(head) head->prev = NULL;
    free(dummyNode);

    return head;
}

SongNode* sortByTitle(SongNode* head) {
    SongNode* newHead = head;
    if(head && head->next) {
        SongNode* head2 = splitLL(head);
        SongNode* t1 = sortByTitle(head);
        SongNode* t2 = sortByTitle(head2);

        newHead = mergeLLByTitle(t1, t2);
    }

    return newHead;
}

void displaySongs(SongNode* playlist) {
    SongNode* temp = playlist;
    printf("\n");
    while(temp) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", temp->songID, temp->title, temp->artist);

        temp = temp->next;
    }

    printf("NULL\n");
}

void deleteLL(SongNode* head) {
    SongNode* temp = head;
    while(temp) {
        SongNode* nextSong = temp->next;
        free(temp);
        temp = nextSong;
    }
}

void displayByTitle(SongNode* playlist) {
    SongNode* copyHead = copyLL(playlist);   //copying the original playlist
    copyHead = sortByTitle(copyHead);  //sorting the copy
    displaySongs(copyHead);
    deleteLL(copyHead);
}

SongNode* mergeLLByArtist(SongNode* head1, SongNode* head2) {
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    SongNode* curr = dummyNode;
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* t1 = head1, *t2 = head2;
    while(t1 && t2) {
        if(strcmp(t1->artist, t2->artist) < 0) {
            curr->next = t1;
            t1->prev = curr;
            curr = t1;
            t1 = t1->next;
        }
        else {
            curr->next = t2;
            t2->prev = curr;
            curr = t2;
            t2 = t2->next;
        }
    }

    if(t1) {
        curr->next = t1;
        t1->prev = curr;
    }
    else if(t2) {
        curr->next = t2;
        t2->prev = curr;
    }

    SongNode* head = dummyNode->next;
    if(head) head->prev = NULL;
    free(dummyNode);

    return head;
}

SongNode* sortByArtist(SongNode* head) {
    SongNode* newHead = head;
    if(head && head->next) {
        SongNode* head2 = splitLL(head);
        SongNode* t1 = sortByArtist(head);
        SongNode* t2 = sortByArtist(head2);

        newHead = mergeLLByArtist(t1, t2);
    }

    return newHead;
}

void displayByArtist(SongNode* playlist) {
    SongNode* copyHead = copyLL(playlist);
    copyHead = sortByArtist(copyHead);
    displaySongs(copyHead);
    deleteLL(copyHead);
}

SongNode* mergeLLByArtistAndTitle(SongNode* head1, SongNode* head2) {
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    SongNode* curr = dummyNode;
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* t1 = head1, *t2 = head2;
    while(t1 && t2) {
        if(strcmp(t1->artist, t2->artist) < 0) {
            curr->next = t1;
            t1->prev = curr;
            curr = t1;
            t1 = t1->next;
        }
        else if(strcmp(t1->artist, t2->artist) > 0) {
            curr->next = t2;
            t2->prev = curr;
            curr = t2;
            t2 = t2->next;
        }
        else {
            if(strcmp(t1->title, t2->title) < 0) {
                curr->next = t1;
                t1->prev = curr;
                curr = t1;
                t1 = t1->next;
            }
            else {
                curr->next = t2;
                t2->prev = curr;
                curr = t2;
                t2 = t2->next;
            }
        }
    }

    if(t1) {
        curr->next = t1;
        t1->prev = curr;
    }
    else if(t2) {
        curr->next = t2;
        t2->prev = curr;
    }

    SongNode* head = dummyNode->next;
    if(head) head->prev = NULL;
    free(dummyNode);

    return head;
}

SongNode* sortByArtistAndTitle(SongNode* head) {
    SongNode* newHead = head;
    if(head && head->next) {
        SongNode* head2 = splitLL(head);
        SongNode* t1 = sortByArtistAndTitle(head);
        SongNode* t2 = sortByArtistAndTitle(head2);

        newHead = mergeLLByArtistAndTitle(t1, t2);
    }

    return newHead;
}

void displayByArtistAndTitle(SongNode* playlist) {
    SongNode* copyHead = copyLL(playlist);
    copyHead = sortByArtistAndTitle(copyHead);
    displaySongs(copyHead);
    deleteLL(copyHead);
}

void handleDisplay(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        printf("\n1.Display by Title.\n2.Display by Artist.\n3.Display by Artist and Title.\n");
        int select;
        printf("\nEnter your choice: ");
        scanf("%d", &select);
        if(select == 1) displayByTitle(playlists[choice-1]);
        else if(select == 2) displayByArtist(playlists[choice-1]);
        else if(select == 3) displayByArtistAndTitle(playlists[choice-1]);
        else printf("\nInvalid choice.\n");
    }
}

void searchByTitle(SongNode* playlist, char title[]) {
    SongNode* temp = playlist;
    printf("\n");
    while(temp) {
        if(strcmp(temp->title, title) == 0) {
            printf("[ID: %d, Title: %s, Artist: %s] -> ", temp->songID, temp->title, temp->artist);
        }
        temp = temp->next;
    }
    printf("NULL\n");
}

void searchByArtist(SongNode* playlist, char artist[]) {
    SongNode* temp = playlist;
    printf("\n");
    while(temp) {
        if(strcmp(temp->artist, artist) == 0) {
            printf("[ID: %d, Title: %s, Artist: %s] -> ", temp->songID, temp->title, temp->artist);
        }
        temp = temp->next;
    }
    printf("NULL\n");

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
            searchByTitle(playlists[choice-1], title);
        }
        else if(select == 3) {
            char artist[100];
            printf("\nEnter the Song Artist: ");
            scanf("%s", artist);
            searchByArtist(playlists[choice-1], artist);
        }
        else {
            printf("\nInvalid choice.\n");
        }
    }
}
void shufflePlaylist(SongNode* head) {
    int n = 0;
    SongNode* temp = head;

    
    while(temp) {
        n++;
        temp = temp->next;
    }

    if(n <= 1) return;

    
    SongNode* copyHead = copyLL(head);
    SongNode* arr[n];
    temp = copyHead;
    for(int i = 0; i < n; i++) {
        arr[i] = temp;
        temp = temp->next;
    }

    
    for(int i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        SongNode* t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }

    
    for(int i = 0; i < n; i++) {
        arr[i]->prev = (i == 0) ? NULL : arr[i-1];
        arr[i]->next = (i == n-1) ? NULL : arr[i+1];
    }

    displaySongs(arr[0]);
    deleteLL(arr[0]);

}

void handleShuffle(SongNode* playlists[], int count) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        shufflePlaylist(playlists[choice-1]);
    }
}



void addToHistory(HistoryNode** history, SongNode* song) {
    HistoryNode* newHead = (HistoryNode*)malloc(sizeof(HistoryNode));
    newHead->songID = song->songID;
    strcpy(newHead->title, song->title);
    strcpy(newHead->artist, song->artist);
    newHead->next = *history;
    *history = newHead;
}

void playSong(SongNode* playlist, HistoryNode** history, int id) {
    SongNode* song = searchByID(playlist, id);
    if(!song) {
        printf("\nSong with ID: %d not found in the Playlist!\n", id);
    }
    else {
        printf("\nPlaying Song: %s.....\n", song->title);
        addToHistory(history, song);
    }
}

void handlePlaySong(SongNode* playlists[], int count, HistoryNode** history) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        int id;
        printf("\nEnter the Song ID: ");
        scanf("%d", &id);
        playSong(playlists[choice-1],history, id);
    }
}

void displayHistoryReverseChronologically(HistoryNode* history) {
    HistoryNode* temp = history;
    printf("\nHistory: \n");
    while(temp) {
        printf("[ID: %d, Title: %s, Artist: %s] -> ", temp->songID, temp->title, temp->artist);

        temp = temp->next;
    }

    printf("NULL\n");
}

void displayHistoryChronologically(HistoryNode* history) {
    if(history) {
        if(!history->next) {
            printf("[ID: %d, Title: %s, Artist: %s] -> ", history->songID, history->title, history->artist);
        }
        else {
            displayHistoryChronologically(history->next);
            printf("[ID: %d, Title: %s, Artist: %s] -> ", history->songID, history->title, history->artist);
        }
    }
}

void handleHistory(SongNode* playlists[], int count, HistoryNode* history) {
    printf("\n1.Display history chronologically.\n2.Display history reverse chronologically.\n");
    int select;
    printf("\nEnter your choice: ");
    scanf("%d", &select);
    if(select == 1) {
        printf("\nHistory:\n");
        displayHistoryChronologically(history);
        printf("NULL\n");
    }
    else if(select == 2) {
        displayHistoryReverseChronologically(history);
    }
    else {
        printf("\nInvalid choice.\n");
    }
}

void handlePlaylist(SongNode* playlists[], int count, HistoryNode** history) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        displaySongs(playlists[choice-1]);
        SongNode* temp = playlists[choice-1];
            while(temp) {
                addToHistory(history, temp);
                temp = temp->next;
            }
    }
}



void handleRepeat(SongNode* playlists[], int count, HistoryNode** history) {
    int choice = choosePlaylist(playlists, count);
    if(choice) {
        int flag = 1, r = 0;
        while(flag  ) {
            displaySongs(playlists[choice-1]);
            SongNode* temp = playlists[choice-1];
            while(temp) {
                addToHistory(history, temp);
                temp = temp->next;
            }
            printf("\n");
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
SongNode* unionOfPlaylist(SongNode* playlist1, SongNode* playlist2) {
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* curr = dummyNode;
    SongNode* t1 = playlist1, *t2 = playlist2;
    while(t1 && t2) {
        SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
        newNode->prev = newNode->next = NULL;
        if(t1->songID < t2->songID) {
            newNode->songID = t1->songID;
            strcpy(newNode->title, t1->title);
            strcpy(newNode->artist, t1->artist);
            curr->next = newNode;
            newNode->prev = curr;
            t1 = t1->next;
        }
        else if(t1->songID > t2->songID) {
            newNode->songID = t2->songID;
            strcpy(newNode->title, t2->title);
            strcpy(newNode->artist, t2->artist);
            curr->next = newNode;
            newNode->prev = curr;
            t2 = t2->next;
        }
        else {
            newNode->songID = t1->songID;
            strcpy(newNode->title, t1->title);
            strcpy(newNode->artist, t1->artist);
            curr->next = newNode;
            newNode->prev = curr;
            t1 = t1->next;
            t2 = t2->next;
        }
        curr = curr->next;
    }
    while(t1) {
        SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
        newNode->prev = newNode->next = NULL;
        newNode->songID = t1->songID;
        strcpy(newNode->title, t1->title);
        strcpy(newNode->artist, t1->artist);
        curr->next = newNode;
        newNode->prev = curr;
        t1 = t1->next;
        curr = curr->next;
    }
    while(t2) {
        SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
        newNode->prev = newNode->next = NULL;
        newNode->songID = t2->songID;
        strcpy(newNode->title, t2->title);
        strcpy(newNode->artist, t2->artist);
        curr->next = newNode;
        newNode->prev = curr;
        t2 = t2->next;
        curr = curr->next;
    }

    SongNode* head = dummyNode->next;
    if(head) head->prev = NULL;
    free(dummyNode);

    return head;
}

SongNode* intersectionOfPlaylist(SongNode* playlist1, SongNode* playlist2) {
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* curr = dummyNode;
    SongNode* t1 = playlist1, *t2 = playlist2;
    while(t1 && t2) {
        if(t1->songID < t2->songID) {
            t1 = t1->next;
        }
        else if(t1->songID > t2->songID) {
            t2 = t2->next;
        }
        else {
            SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
            newNode->prev = newNode->next = NULL;
            newNode->songID = t1->songID;
            strcpy(newNode->title, t1->title);
            strcpy(newNode->artist, t1->artist);
            curr->next = newNode;
            newNode->prev = curr;
            t1 = t1->next;
            t2 = t2->next;
            curr = curr->next;
        }
    }

    SongNode* head = dummyNode->next;
    if(head) head->prev = NULL;
    free(dummyNode);

    return head;
}

SongNode* differenceOfPlaylists(SongNode* playlist1, SongNode* playlist2) {
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* curr = dummyNode;
    SongNode* t1 = playlist1, *t2 = playlist2;
    while(t1 && t2) {
        if(t1->songID < t2->songID) {
            SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
            newNode->prev = newNode->next = NULL;
            newNode->songID = t1->songID;
            strcpy(newNode->title, t1->title);
            strcpy(newNode->artist, t1->artist);
            curr->next = newNode;
            newNode->prev = curr;
            t1 = t1->next;
            curr = curr->next;
        }
        else if(t1->songID > t2->songID) {
            t2 = t2->next;
        }
        else {
            t1 = t1->next;
            t2 = t2->next;
        }
    }

    while(t1) {
        SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
        newNode->prev = newNode->next = NULL;
        newNode->songID = t1->songID;
        strcpy(newNode->title, t1->title);
        strcpy(newNode->artist, t1->artist);
        curr->next = newNode;
        newNode->prev = curr;
        t1 = t1->next;
        curr = curr->next;
    }
    
    SongNode* head = dummyNode->next;
    if(head) head->prev = NULL;
    free(dummyNode);

    return head;
}

SongNode* symmetricDifferenceOfPlaylists(SongNode* playlist1, SongNode* playlist2) {
    SongNode* dummyNode = (SongNode*)malloc(sizeof(SongNode));
    dummyNode->prev = dummyNode->next = NULL;
    SongNode* curr = dummyNode;
    SongNode* t1 = playlist1, *t2 = playlist2;
    while(t1 && t2) {
        if(t1->songID < t2->songID) {
            SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
            newNode->prev = newNode->next = NULL;
            newNode->songID = t1->songID;
            strcpy(newNode->title, t1->title);
            strcpy(newNode->artist, t1->artist);
            curr->next = newNode;
            newNode->prev = curr;
            t1 = t1->next;
            curr = curr->next;
        }
        else if(t1->songID > t2->songID) {
            SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
            newNode->prev = newNode->next = NULL;
            newNode->songID = t2->songID;
            strcpy(newNode->title, t2->title);
            strcpy(newNode->artist, t2->artist);
            curr->next = newNode;
            newNode->prev = curr;
            t2 = t2->next;
            curr = curr->next;
        }
        else {
            t1 = t1->next;
            t2 = t2->next;
        }
    }

    while(t1) {
        SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
        newNode->prev = newNode->next = NULL;
        newNode->songID = t1->songID;
        strcpy(newNode->title, t1->title);
        strcpy(newNode->artist, t1->artist);
        curr->next = newNode;
        newNode->prev = curr;
        t1 = t1->next;
        curr = curr->next;
    }

    while(t2) {
        SongNode* newNode = (SongNode*)malloc(sizeof(SongNode));
        newNode->prev = newNode->next = NULL;
        newNode->songID = t2->songID;
        strcpy(newNode->title, t2->title);
        strcpy(newNode->artist, t2->artist);
        curr->next = newNode;
        newNode->prev = curr;
        t2 = t2->next;
        curr = curr->next;
    }

    
    SongNode* head = dummyNode->next;
    if(head) head->prev = NULL;
    free(dummyNode);

    return head;
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
                            printf("\nIntersecton:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;

                        case 3:
                            printf("\nDifference done successfully!\n");
                            playlists[*count] = differenceOfPlaylists(playlists[choice1-1], playlists[choice2-1]);
                            printf("\nDifference:\n");
                            displaySongs(playlists[*count]);
                            (*count)++;
                            break;

                        case 4:
                            printf("\nSymmetric Difference done successfully!\n");
                            playlists[*count] = symmetricDifferenceOfPlaylists(playlists[choice1-1], playlists[choice2-1]);
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

void freePlaylists(SongNode* playlists[], int count) {
    for(int i=0;i<count;i++) {
        SongNode* temp = playlists[i];
        while(temp) {
            SongNode* nextSong = temp->next;
            free(temp);
            temp = nextSong;
        }
        playlists[i] = NULL;
    }
}

int main() {
    SongNode* playlists[10];
    HistoryNode* history = NULL;
    int done = 0;
    int playlistCount = 0;

    while(!done) {
        printf("\n1.Add a song.\n2.Delete a song.\n3.Display Playlist.\n4.Search a song.\n5.Shuffle Mode.\n6.Repeat Mode.\n7.Play a Playlist.\n8.Play a song.\n9.Display History.\n10.Operations on multiple playlists.\n11.Exit.\n");

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
                handleRepeat(playlists, playlistCount, &history);
                break;
            case 7:
                handlePlaylist(playlists, playlistCount, &history);
                break;

            case 8:
                handlePlaySong(playlists, playlistCount, &history);
                break;

            case 9:
                handleHistory(playlists, playlistCount, history);
                break;

            case 10:
                handleMultiplePlaylists(playlists, &playlistCount);
                break;
            
            case 11:
                freePlaylists(playlists, playlistCount);
                HistoryNode* temp = history;
                while(temp) {
                    free(temp);
                    temp = temp->next;
                }
                done = 1;
                break;

            default:
                printf("\nInvalid choice.\n");
                break;
        }

        
    }
    

}