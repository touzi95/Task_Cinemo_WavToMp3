/***
 * Author: TOUZI Tayeb 
 * Date:   20/12/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <pthread.h>

/* Lame Include */
#include "lame.h"

/* global var for threading */
int g=0;
struct arg_struct {
    char         *mp3_output_file;
    char         *mp3_input_file;
};


static void *wav_mp3_thread (void *input) // need data argc et argv
{
	struct arg_struct   *test = (struct arg_struct *) input ;
	unsigned int sampleRate = 16000;  /*assumed.*/
	unsigned int nSecondsAudio = 4;
	float *arr;
	lame_global_flags *gfp;
	unsigned char mp3buffer[2304]; /*some odd buffer sizes hard-coded.*/
	int pcm_samples_1d[2*1152];
	int pcm_samples_2d[2][1152];
	int read = 0, write = 0;
	int return_code = 1;
	int mp3buf_size;
	FILE *mp3 = fopen(test->mp3_output_file , "w");
	FILE *pcm = fopen(test->mp3_input_file , "r");
	int framesize = 0;
	int i = 0, j = 0, num_samples_encoded = 0;
	
	/*Show encoder version.*/
	char *s = malloc(200 * sizeof *s);
	s = strdup(get_lame_version());
	printf("Lame version = %s\n", s);
	
	
	/* Init lame flags.*/
	gfp = lame_init();
	if(!gfp) {
		printf("Unable to initialize gfp object.");
	} else {
		printf("Able to initialize gfp object.\n");
	}
	
	/* set other parameters.*/
	lame_set_num_channels(gfp, 1);
	
	/*lame_set_num_samples(gfp, (nSecondsAudio * sampleRate));*/
	lame_set_in_samplerate(gfp, sampleRate);
	lame_set_quality(gfp, 5);  /* set for high speed and good quality. */
	lame_set_mode(gfp, 3);  /* the input audio is mono */
	
	lame_set_out_samplerate(gfp, sampleRate);
	printf("Able to set a number of parameters too.");
	framesize = lame_get_framesize(gfp);
	printf("Framesize = %d\n", framesize);
	assert(framesize <= 1152);
	
	/* set more internal variables. check for failure.*/
	if(lame_init_params(gfp) == -1) {
		printf("Something failed in setting internal parameters.");
	}
	
	/* encode the pcm array as mp3.*
	* Read the file. Encode whatever is read.
	* As soon as end of file is reached, flush the buffers.
	* Write everything to a file.
	* Write headers too.
	*/
	
	do {
		read = fread(pcm_samples_1d, sizeof(short), 2304, pcm); /*reads framesize shorts from pcm file.*/
		printf("Read %d shorts from file.\n", read);
		
		/* check for number of samples read. if 0, start flushing, else encode.*/
		if(read > 0) {
		 /* got data in 1D array. convert it to 2D */
		 /* snippet below taken from lame source code. needs better understanding. pcm_samples_2d[0] = contents of buffer. pcm_samples_2d[1] = 0 since number of channels is always one.*/
		 memset(pcm_samples_2d[1], 0, 1152 * sizeof(int));  /*set all other samples with 0.*/
		 memset(pcm_samples_2d[0], 0, 1152 * sizeof(int));
		 i = 0, j = 0;
		 for(i = 0; i < 1152; i++) {
		   pcm_samples_2d[0][i] = pcm_samples_1d[i];
		 }
		
		 /* encode samples. */
		 num_samples_encoded = lame_encode_buffer(gfp, (const short int *) pcm_samples_1d, NULL, read, mp3buffer, sizeof(mp3buffer));
		
		 printf("number of samples encoded = %d\n", num_samples_encoded);
		
		 /* check for value returned.*/
		 if(num_samples_encoded > 1) {
			printf("It seems the conversion was successful.\n");
		 } else if(num_samples_encoded == -1) {
			printf("mp3buf was too small");
			pthread_exit(NULL);
		 } else if(num_samples_encoded == -2) {
			printf("There was a malloc problem.");
			pthread_exit(NULL);
		 } else if(num_samples_encoded == -3) {
			printf("lame_init_params() not called.");
			pthread_exit(NULL);
		 } else if(num_samples_encoded == -4) {
			printf("Psycho acoustic problems.");
			pthread_exit(NULL);
		 } else {
			printf("The conversion was not successful.");
			pthread_exit(NULL);
		 }
		
		 printf("Contents of mp3buffer = \n");
		 for(i = 0; i < 2304; i++) {
			printf("mp3buffer[%d] = %d\n", i, mp3buffer[i]);
		 }
		
		
		 write = (int) fwrite(mp3buffer, sizeof(char), num_samples_encoded, mp3);
		 if(write != num_samples_encoded) {
		   printf("There seems to have been an error writing to mp3 within the loop.\n");
		   pthread_exit(NULL);
		 } else {
		   printf("Writing of %d samples a success.\n", write);
		 }
		}
	} while(read > 0);
	
	/* in case where the number of samples read is 0, or negative, start flushing.*/
	read = lame_encode_flush(gfp, mp3buffer, sizeof(mp3buffer)); /*this may yield one more mp3 buffer.*/
	if(read < 0) {
		if(read == -1) {
			printf("mp3buffer is probably not big enough.\n");
		} else {
			printf("MP3 internal error.\n");
		}
		pthread_exit(NULL);
	} else {
		printf("Flushing stage yielded %d frames.\n", read);
	}
	
	write = (int) fwrite(mp3buffer, 1, read, mp3);
	if(write != read) {
		printf("There seems to have been an error writing to mp3.\n");
		pthread_exit(NULL);
	}
	
	/*samples have been written. write ID3 tag.*/
	read = lame_get_id3v1_tag(gfp, mp3buffer, sizeof(mp3buffer));
	if(sizeof(read) > sizeof(mp3buffer)) {
		printf("Buffer too small to write ID3v1 tag.\n");
	} else {
		if(read > 0) {
		  write = (int) fwrite(mp3buffer, 1, read, mp3);
		  if(read != write) {
			printf("more errors in writing id tag to mp3 file.\n");
		  }
	    }
	}
	
	lame_close(gfp);
	fclose(pcm);
	fclose(mp3);
	test = NULL;
}
	


int main(int argc, char **argv)
{

    int           k;
    int           NB_FILES=argc - 1	;
	size_t        input_file_length = 0;
    char          format_input[3];
    pthread_t  	  threads[NB_FILES];
    int  		  rc;	 /* threading error var */
    
    /* check the number of input files to treat */
	if (argc < 2) {
		printf("Bad params\n");
		printf("wav2mp3 <wav_file(s)> \n");
		return 1;
	}

    /* check the format of the input files : wav or not */
    for ( k=1; k<=NB_FILES;k++){
		input_file_length=strlen(argv[k]);
		strncpy (format_input, argv[k]+input_file_length-3, 4);
		if ( (strcmp ( format_input, "wav") != 0 ) ) {
			printf("Bad input file, it should be a wav file and yours is %s : file causing prob is %s\n",format_input,argv[k]);
			return 1;
		}  
	}
	
	/* Multi-threading for convrsion process */ 
	for (k = 0; k < NB_FILES; k++) {
		
		struct arg_struct *args = (struct arg_struct *)malloc (sizeof(struct arg_struct));
		input_file_length=strlen(argv[k+1]);
		
		/* copying files info to args ( that we will pass in thread creating */
		args->mp3_input_file = (char *) malloc(input_file_length);
		args->mp3_output_file = (char *) malloc(input_file_length);
		char * mp3_output_file = (char *) malloc(input_file_length);
		char * mp3_input_file = (char *) malloc(input_file_length);
		strncpy(mp3_output_file, argv[k+1], input_file_length); /* -copy input path in the output to change the extension to .mp3 */
		strtok(mp3_output_file, ".");  /* we need to treat the case when we have more then 1 point in the path file --> use while loop */
		strcat(mp3_output_file,".mp3");
		// printf("Output: %s\n", mp3_output_file);
		strncpy(mp3_input_file, argv[k+1], input_file_length); /* i,put file for multithreading function */
		// printf("Input : %s\n", mp3_input_file);
		strcpy(args->mp3_input_file,(char *)argv[k+1]);
		strcpy(args->mp3_output_file,mp3_output_file);
				
		rc = pthread_create (&(threads[k]), NULL,wav_mp3_thread,( void * )args);
		
		if (rc) {
			fprintf (stderr, "%s \n", strerror (rc));
		}
	}
	/* waiting for threads to finish their jobs */   
    for (k = 0; k < NB_FILES; k++) {
		pthread_join (threads[k], NULL);
    }   
    return 1;
}
