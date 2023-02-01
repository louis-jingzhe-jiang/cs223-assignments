/**
* A program that rills an array with three or more songs and then allows the 
* user to edit the data in the array.
*
* @author: Louis Jiang
* @version: Jan 28, 2023
*/

#include <stdio.h>
#include <string.h>

struct song {
  char name[32];
  char artist[32];
  int min;
  int sec;
  float danceability;
};

void print_info(struct song songList[], int length) {
  for (int i = 0; i < length; i++) {
    printf("%i) %-32sartist: %-20sduration: %02i:%02i%-15sdanceability: %.2f\n",
      i, songList[i].name, songList[i].artist, songList[i].min, 
      songList[i].sec, " ", songList[i].danceability);
  }
}

int main() {
  struct song songList[6]; // create the song struct array
  // enter the infomation of four default songs
  strcpy(songList[0].name, "Faded");
  strcpy(songList[0].artist, "Alan Walker");
  songList[0].min = 3;
  songList[0].sec = 33;
  songList[0].danceability = 0.5;
  strcpy(songList[1].name, "Someone Like You");
  strcpy(songList[1].artist, "Adele");
  songList[1].min = 4;
  songList[1].sec = 45;
  songList[1].danceability = 0.4;
  strcpy(songList[2].name, "Yesterday");
  strcpy(songList[2].artist, "The Beatles");
  songList[2].min = 2;
  songList[2].sec = 5;
  songList[2].danceability = 0.6;
  strcpy(songList[3].name, "Shape of You");
  strcpy(songList[3].artist, "Ed Sheeran");;
  songList[3].min = 3;
  songList[3].sec = 54;
  songList[3].danceability = 0.8;
  
  // the length of the song struct array
  int length = 4;
  // print out the welcoming info and song list
  print_info(songList, length);
  // show editing info
  printf("Enter a song id to edit [0, 1, 2, 3]: ");
  int edit;
  scanf(" %i", &edit);
  if (edit < 0 || edit >= length) { // invalid input, return
    printf("This is an invalid input!\n");
    return 0;
  }
  printf("Which attribute do you wish to edit? ");
  printf("[artist, title, duration, danceability]: ");
  char attribute[16];
  scanf(" %s", attribute);
  // deal with different attribute entries
  if (! strcmp(attribute, "artist")) { // change artist
    printf("Enter artist: ");
    char new_artist[32];
    scanf(" %[^\n]%*c", new_artist);
    strcpy(songList[edit].artist, new_artist);
  } else if (! strcmp(attribute, "title")) { // change title
    printf("Enter title: ");
    char new_name[32];
    scanf(" %[^\n]%*c", new_name);
    strcpy(songList[edit].name, new_name);
  } else if (! strcmp(attribute, "duration")) { // change duration
    printf("Enter duration (minutes): ");
    int new_min;
    printf("Enter duration (seconds): ");
    scanf(" %i", &new_min);
    int new_sec;
    scanf(" %i", &new_sec);
    songList[edit].min = new_min;
    songList[edit].sec = new_sec;
  } else if (! strcmp(attribute, "danceability")) { // change danceability
    printf("Enter danceability: ");
    float new_danceability;
    scanf(" %f", &new_danceability);
    songList[edit].danceability = new_danceability;
  } else { // invalid user input
    printf("This is an invalid input!\n");
    return 0;
  }
  print_info(songList, length);
  return 0;
}
