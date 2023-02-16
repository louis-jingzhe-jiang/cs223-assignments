/*----------------------------------------------
 * Author: Louis Jiang
 * Date: Feb 15, 2023
 * Description:
 * A program that allows users to easilly access the most danceable song in
 * songlist.csv
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


struct node {
  struct song* s;
  struct node* next;
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


/**
 * Print out the info about a single song
 *
 * @param s the song
 */
void print_single(struct song* s) {
  printf("%-32sArtist: %-20sDuration: %02i:%02i    Danceability: %.03f",
    s->title, s->artist, s->min, s->sec, s->danceability);
  printf("    Energy: %.03f    Tempo: %.03f    Valence: %.03f\n",
    s->energy, s->tempo, s->valence);
}


/**
 * The function used to show all remaining song infomation in the linked list
 * 
 * @param first the first node in the linked list
 */
void display(struct node* first) {
  int count = 0;
  while (first) {
    printf("%02i) ", count);
    print_single(first->s);
    first = first->next;
    count++;
  }
  printf("\nDataset contains %i songs\n\n", count);
  printf("=================================================\n");
}


/**
 * Find the index of the song with the highest danceability in the linked
 * list of songs.
 * 
 * @param first the first node in the linked list
 * 
 * @return the index of the song with the highest danceability
 */
int find_most_danceable(struct node* first) {
  float danceability = first->s->danceability;
  first = first->next;
  int i = 1;
  int result = 0;
  while (first) {
    if (first->s->danceability > danceability) {
      result = i;
    }
    first = first->next;
    i++;
  }
  return result;
}


int main() {
  struct node* first;
  struct node* second = NULL;
  // dealing with file reading
  FILE* infile = NULL;
  infile = fopen("songlist.csv", "r");
  if (infile == NULL) {
    printf("Error: unable to open file %s\n", "songlist.csv");
    exit(1);
  }
  // read the data in the first line (includes the number of songs)
  char buffer[1024];
  // skip the first line
  fgets(buffer, 1024, infile);
  // now read in data line by line
  char* token;
  int ms;
  while (fgets(buffer, 1024, infile)) {
    struct song* this_song = malloc(sizeof(struct song));
    // tokenize the line
    token = strtok(buffer, ",");
    this_song->title = malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(this_song->title, token);
    token = strtok(NULL, ",");
    this_song->artist = malloc(sizeof(char) * (strlen(token) + 1));
    strcpy(this_song->artist, token);
    token = strtok(NULL, ",");
    ms = atoi(token);
    convert(ms, &this_song->min, &this_song->sec);
    token = strtok(NULL, ",");
    this_song->danceability = atof(token);
    token = strtok(NULL, ",");
    this_song->energy = atof(token);
    token = strtok(NULL, ",");
    this_song->tempo = atof(token);
    token = strtok(NULL, ",");
    this_song->valence = atof(token);
    //damn
    first = malloc(sizeof(struct node));
    first->s = this_song;
    first->next = second;
    second = first; // second is also the first node after data reading
  }
  fclose(infile); // close the file
  
  // start the interaction with user
  display(first);
  printf("Press 'd' to show the most danceable song (any other key to quit): ");
  char user_input[32];
  scanf(" %s", user_input);
  while (strlen(user_input) == 1 && user_input[0] == 'd') {
    // check whether there are any songs left in the linked list
    if (first) { // there are songs in the linked list
      // find the index for the  most danceable song
      int most = find_most_danceable(first);
      // locate the node
      struct node* target = first;
      struct node* previous = NULL;
      struct node* next = first->next;
      for (int i = 0; i < most; i++) {
        previous = target;
        target = next;
        if (next) {
          next = next->next;
        }
      }
      // get the song
      struct song* will_display = target->s;
      // display the song
      printf("----------------------------------------------------\n");
      print_single(will_display);
      printf("----------------------------------------------------\n");
      // free the memory of the song
      free(will_display->artist);
      free(will_display->title);
      free(will_display);
      // remove the node from the linked list
      if (previous == NULL) {
        first = next;
      } else if (next == NULL) {
        previous->next = NULL;
      } else {
        previous->next = next;
      }
      free(target);
    }
    display(first);
    // update user input
    printf("Press 'd' to show the most danceable song");
    printf(" (any other key to quit): ");
    scanf(" %s", user_input);
  }
  /*
  // free the memory
  while (first != NULL) {
    // free the memory of artist and title
    free(first->s->artist);
    free(first->s->title);
    // free the memory of the song
    free(first->s);
    second = first->next;
    // free the memory of the linked list node
    free(first);
    // set the next node to be freed
    first = second;
  } // TESTED PASSED!
  */
  return 0;
}
