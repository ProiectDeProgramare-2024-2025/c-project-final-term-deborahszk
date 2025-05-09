#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define MAX_LEN 50
#define FILE_IN "playlists_in.txt"
#define FILE_OUT "playlists_out.txt"

#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"
#define MAGENTA "\x1B[35m"

typedef struct 
{
    char name[MAX_LEN];
    float rating;
} Song;

typedef struct 
{
    char name[MAX_LEN];
    float rating;
    Song songs[MAX];
    int song_count;
} Playlist;

Playlist playlists[MAX];
int playlist_count = 0;

void clr() {
    while (getchar() != '\n');
}

void clear_screen()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int compare_songs(const void *a, const void *b)
{
    Song *sa = (Song *)a;
    Song *sb = (Song *)b;
    return (sb->rating > sa->rating) - (sb->rating < sa->rating);
}

int compare_playlists(const void *a, const void *b)
{
    Playlist *pa = (Playlist *)a;
    Playlist *pb = (Playlist *)b;
    return (pb->rating > pa->rating) - (pb->rating < pa->rating);
}

void sort_songs(Playlist *pl)
{
    qsort(pl->songs, pl->song_count, sizeof(Song), compare_songs);
}

void sort_playlists()
{
    qsort(playlists, playlist_count, sizeof(Playlist), compare_playlists);
}

void load_from_file()
{
    FILE *f = fopen(FILE_IN, "r");
    if (!f)
    {
        printf(RED "No input file found. Starting fresh.\n" RESET);
        return;
    }

    if (fscanf(f, "%d\n", &playlist_count) != 1)
    {
        printf(RED "Corrupted input file. Starting fresh.\n" RESET);
        playlist_count = 0;
        fclose(f);
        return;
    }

    for (int i = 0; i < playlist_count; i++)
    {
        fgets(playlists[i].name, MAX_LEN, f);
        playlists[i].name[strcspn(playlists[i].name, "\n")] = 0;
        fscanf(f, "%f\n", &playlists[i].rating);
        fscanf(f, "%d\n", &playlists[i].song_count);
        for (int j = 0; j < playlists[i].song_count; j++)
            fscanf(f, "%49s %f\n", playlists[i].songs[j].name, &playlists[i].songs[j].rating);
        sort_songs(&playlists[i]);
    }
    sort_playlists();
    fclose(f);
}

void save_to_file()
{
    FILE *f = fopen(FILE_OUT, "w");
    if (!f)
    {
        perror("Could not write to output file");
        return;
    }

    fprintf(f, "%d\n", playlist_count);
    for (int i = 0; i < playlist_count; i++)
    {
        fprintf(f, "%s\n", playlists[i].name);
        fprintf(f, "%.2f\n", playlists[i].rating);
        fprintf(f, "%d\n", playlists[i].song_count);
        for (int j = 0; j < playlists[i].song_count; j++)
            fprintf(f, "%s %.2f\n", playlists[i].songs[j].name, playlists[i].songs[j].rating);
    }

    fclose(f);
}

int is_valid_string(char *str)
{
    if (str == NULL || strlen(str) == 0) return 0;
    for (int i = 0; i < strlen(str); i++)
        if (!isalnum(str[i]) && str[i] != ' ') return 0;
    return 1;
}

void add_playlist()
{
    if (playlist_count >= MAX)
    {
        printf(RED "Max number of playlists reached!\n" RESET);
        return;
    }
    char playlist_name[MAX_LEN];
    printf("Playlist name: ");
    fgets(playlist_name, MAX_LEN, stdin);
    playlist_name[strcspn(playlist_name, "\n")] = 0;
    if (!is_valid_string(playlist_name))
    {
        printf(RED "Invalid playlist name! Only alphanumeric characters and spaces allowed.\n" RESET);
        return;
    }

    float rating;
    printf("Rating (0-5): ");
    while (scanf("%f", &rating) != 1 || rating < 0 || rating > 5)
    {
        printf(RED "Invalid rating. Try again (0-5): " RESET);
        clr();
    }
    clr();

    strcpy(playlists[playlist_count].name, playlist_name);
    playlists[playlist_count].rating = rating;
    playlists[playlist_count].song_count = 0;
    playlist_count++;

    sort_playlists();
    save_to_file();
    printf(GREEN "Playlist added successfully!\n" RESET);
}

void show_playlists()
{
    if (playlist_count == 0)
    {
        printf(RED "No playlists.\n" RESET);
        return;
    }
    sort_playlists();
    for (int i = 0; i < playlist_count; i++)
        printf("%d. " BLUE "%s" RESET " - %.2f (%d songs)\n", i + 1, playlists[i].name, playlists[i].rating, playlists[i].song_count);
}

void delete_playlist()
{
    if (playlist_count == 0)
    {
        printf(RED "No playlists to delete.\n" RESET);
        return;
    }

    int idx;
    printf("Playlist number to delete: ");
    while (scanf("%d", &idx) != 1 || idx < 1 || idx > playlist_count)
    {
        printf("Invalid input. Try again: ");
        clr();
    }
    clr();

    for (int i = idx - 1; i < playlist_count - 1; i++)
        playlists[i] = playlists[i + 1];
    playlist_count--;
    sort_playlists();
    save_to_file();
    printf(GREEN "Playlist deleted.\n" RESET);
}

void add_song(Playlist *pl)
{
    if (pl->song_count >= MAX)
    {
        printf(RED "Playlist full!\n" RESET);
        return;
    }
    char song_name[MAX_LEN];
    printf("Song name: ");
    fgets(song_name, MAX_LEN, stdin);
    song_name[strcspn(song_name, "\n")] = 0;

    if (!is_valid_string(song_name))
    {
        printf(RED "Invalid song name! Only alphanumeric characters and spaces allowed.\n" RESET);
        return;
    }

    float rating;
    printf("Rating (0-5): ");
    while (scanf("%f", &rating) != 1 || rating < 0 || rating > 5)
    {
        printf(RED "Invalid input. Try again (0-5): " RESET);
        clr();
    }
    clr();

    strcpy(pl->songs[pl->song_count].name, song_name);
    pl->songs[pl->song_count].rating = rating;
    pl->song_count++;
    sort_songs(pl);
    save_to_file();
    printf(GREEN "Song added successfully!\n" RESET);
}

void show_songs(Playlist *pl)
{
    if (pl->song_count == 0)
    {
        printf(RED "No songs in this playlist.\n" RESET);
        return;
    }
    sort_songs(pl);
    for (int i = 0; i < pl->song_count; i++)
        printf("%d. " MAGENTA "%s" RESET " - %.2f\n", i + 1, pl->songs[i].name, pl->songs[i].rating);
}

void delete_song(Playlist *pl)
{
    if (pl->song_count == 0) 
    {
        printf(RED "No songs to delete.\n" RESET);
        return;
    }
    int idx;
    printf("Song number to delete: ");
    while (scanf("%d", &idx) != 1 || idx < 1 || idx > pl->song_count)
    {
        printf("Invalid input. Try again: ");
        clr();
    }
    clr();
    for (int i = idx - 1; i < pl->song_count - 1; i++)
        pl->songs[i] = pl->songs[i + 1];
    pl->song_count--;
    sort_songs(pl);
    save_to_file();
    printf(GREEN "Song deleted!\n" RESET);
}

void playlist_menu(Playlist *pl)
{
    int choice;
    do
    {
        clear_screen();
        printf("--- Playlist: " BLUE "%s" RESET " (Rating: %.2f) ---\n", pl->name, pl->rating);
        printf("1 - Add Song\n");
        printf("2 - Show Songs\n");
        printf("3 - Delete Song\n");
        printf("0 - Back\n");
        printf(">> ");
        while (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Try again: ");
            clr();
        }
        clr();
        switch (choice)
        {
            case 1: add_song(pl); break;
            case 2: show_songs(pl); break;
            case 3: delete_song(pl); break;
            case 0: break;
            default: printf(RED "Invalid choice!\n" RESET);
        }
        if (choice != 0)
        {
            printf("\n(Press Enter to continue...)");
            clr();
        }
    } while (choice != 0);
}

void main_menu()
{
    int choice;
    do
    {
        clear_screen();
        printf("--- Playlist Manager ---\n");
        printf("1 - Add Playlist\n");
        printf("2 - Show Playlists\n");
        printf("3 - Delete Playlist\n");
        printf("4 - Manage Playlist\n");
        printf("0 - Exit\n");
        printf(">> ");
        while (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Try again: ");
            clr();
        }
        clr();
        switch (choice)
        {
            case 1: add_playlist(); break;
            case 2: show_playlists(); break;
            case 3: delete_playlist(); break;
            case 4:
                if (playlist_count == 0)
                {
                    printf(RED "No playlists to manage.\n" RESET);
                    break;
                }
                show_playlists();
                int idx;
                printf("Select playlist number: ");
                while (scanf("%d", &idx) != 1 || idx < 1 || idx > playlist_count)
                {
                    printf("Invalid input. Try again: ");
                    clr();
                }
                clr();
                playlist_menu(&playlists[idx - 1]);
                break;
            case 0:
                printf(GREEN "Goodbye!\n" RESET);
                break;
            default:
                printf(RED "Invalid choice!\n" RESET);
        }
        if (choice != 0)
            printf("\n(Press Enter to continue...)"); getchar();
    } while (choice != 0);
}

int main()
{
    clear_screen();
    load_from_file();
    main_menu();
    return 0;
}
