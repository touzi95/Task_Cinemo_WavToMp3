Task : 

Write a C/C++ commandline application that encodes a set of WAV files to MP3

Reached Requirements 

  -   application is called with pathname as argument, e.g. <applicationname> F:\MyWavCollection all WAV-files contained directly in that folder are to be encoded to MP3
  -   statically link to lame encoder library
  -   application should be compilable and runnable only on Linux environment
  -   the resulting MP3 files are to be placed within the same directory as the source WAV files, the filename extension should be changed appropriately to .MP3 ( just for one file )
  -   non-WAV files in the given folder shall be ignored ( with a warning message that the input is not a wav file )
  -   frameworks such as Boost or Qt shall not be used ( respected )  
  -   the LAME encoder should be used with reasonable standard settings (e.g. quality based encoding with quality level "good") : probleme with the mp3 output sound ( noise ) !!

Remaining to complete :

   -  Make the app runnable on Windows --> i use MinGW, the cross-compiler but i got an error telling me can't use the option "-rdynamic" i have to work on it ( so i canceled this part )

How To:

   - Create an empty folder called build : mkdir build
   - cd build/
   - cmake ../ && sudo make
   - ./wav2mp3 <wav_files_path>    check the mp3 file will be added in the same path with the same name ( different format ).

