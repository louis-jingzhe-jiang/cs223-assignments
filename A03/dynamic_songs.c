/*----------------------------------------------
 * Author: Louis Jiang
 * Date: Feb 6, 2023
 * Description
 * This program reads info about songs from a csv file into an array and print
 * out the list of the songs.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct song {
  char* title;
  char* artist;
  int min;
  int sec;
  float danceability;
  float energy;
  float tempo;
  float valence;
};


/**
 * Convert a time in milliseconds to minute:second
 *
 * @param ms the time in milliseconds
 * @param *min the pointer to the minute
 * @param *sec the pointer to the second
 */
void convert(int ms, int* min, int* sec) {
  ms = ms / 1000;
  *min = ms / 60;
  *sec = ms % 60;
}


int main() {
  struct song* songList;
  
  // dealing with file reading
  FILE* infile = NULL;
  infile = fopen("songlist.csv", "r");
  if (infile == NULL) {
    printf("Error: unable to open file %s\n", "songlist.csv");
    exit(1);
  }
  // read the data in the first line (includes the number of songs)
  char buffer[1024];
  fgets(buffer, 1024, infile);
  int song_num = atoi(strtok(buffer, ",")); // the number of songs
  // allocate memory for a list of songs
  songList = malloc(sizeof(struct song) * song_num);
  // printf("%i\n", song_num); // test passed! song_num works as intended
  // pass the second line (since it does not contain useful info)
  fgets(buffer, 1024, infile);
  // now read in data line by line
  char* token;
  int ms;
  for (int i = 0; i < song_num; i++) {
    fgets(buffer, 1024, infile); // read line
    // tokenize the line
    token = strtok(buffer, ",");
    songList[i].title = malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(songList[i].title, token);
    token = strtok(NULL, ",");
    songList[i].artist = malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(songList[i].artist, token);
    token = strtok(NULL, ",");
    ms = atoi(token);
    convert(ms, &songList[i].min, &songList[i].sec);
    token = strtok(NULL, ",");
    songList[i].danceability = atof(token);
    token = strtok(NULL, ",");
    songList[i].energy = atof(token);
    token = strtok(NULL, ",");
    songList[i].tempo = atof(token);
    token = strtok(NULL, ",");
    songList[i].valence = atof(token);
  }
  fclose(infile); // close the file
  
  // start to print out the result
  printf("Welcome to Dnamic Donna's Danceability Directory.\n\n");
  for (int i = 0; i < song_num; i++) {
    printf("%02i) %-32sArtist: %-20sDuration: %02i:%02i    Danceability: %.03f",
      i, songList[i].title, songList[i].artist, songList[i].min,
      songList[i].sec, songList[i].danceability);
    printf("    Energy: %.03f    Tempo: %.03f    Valence: %.03f\n\n",
      songList[i].energy, songList[i].tempo, songList[i].valence);
  }

  // freeing the songlist
  for (int i = 0; i < song_num; i++) {
    free(songList[i].title);
    free(songList[i].artist);
  }
  free(songList);
  return 0;
}
