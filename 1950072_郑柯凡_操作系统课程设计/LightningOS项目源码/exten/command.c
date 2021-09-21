/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		               Kefan-Zheng,  2021   		
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

void introduction()
{
	printf(" ==============================================================================\n");
	printf(" # Operating System Course Design                                             #\n");
	printf(" # @author: 1950072 kefan Zheng                                               #\n");
	printf(" # @teacher: teacher Wang                                                     #\n");
	printf(" # @time: 2021.8                                                              #\n");
	printf(" # Tongji SSE                                                                 #\n");
	printf(" ==============================================================================\n");
	printf("\n");
}

void noSuchCom()
{
	printf(" Sorry. There's no such command.\n"); 
	printf(" You can get more information by the command [help].");
	printf("\n\n");
}


void commandList()
{
	clear();
	printf(" ==============================================================================\n");
	printf(" #  *Lightning OS*                                               TONGJI UNI   #\n");
	printf(" #                                                                 kefan.Z    #\n");
	printf(" #  [COMMAND LIST]                                            *               #\n");
	printf(" #                                                          **                #\n");
	printf(" #  # help --- show the command list                      ***                 #\n");
	printf(" #                                                      ****                  #\n");
	printf(" #  # clear --- clear the console                     *****                   #\n");
	printf(" #                                                  ******                    #\n");
	printf(" #  # transnum --- transform num system           **************              #\n");
	printf(" #                                              **************                #\n");
	printf(" #  # calculator --- run calculator           **************                  #\n");
	printf(" #                                                  ******                    #\n");
	printf(" #  # ps --- show the process                      *****                      #\n");
	printf(" #                                                ****                        #\n");
	printf(" #  # fs --help --- show file command            ***                          #\n");
	printf(" #                                              **                            #\n");
	printf(" #  # game --- 4 games                         *                              #\n");
	printf(" #                                                                            #\n");
	printf(" #                                                                            #\n");
	printf(" ==============================================================================\n");	    
	printf("\n");
}

void gameList()
{
	clear();
	printf(" ==============================================================================\n");
	printf(" #  *Lightning OS*                                               TONGJI UNI   #\n");
	printf(" #                                                                 kefan.Z    #\n");
	printf(" #  [GAME LIST]                                               *               #\n");
	printf(" #                                                          **                #\n");
	printf(" #  # draughts -- play draughts                           ***                 #\n");
	printf(" #                                                      ****                  #\n");
	printf(" #  # 2048 -- play 2048                               *****                   #\n");
	printf(" #                                                  ******                    #\n");
	printf(" #  # snake -- play snake                         **************              #\n");
	printf(" #                                              **************                #\n");
	printf(" #  # gomoku -- play gomoku                   **************                  #\n");
	printf(" #                                                  ******                    #\n");
	printf(" #  @Attention                                     *****                      #\n");
	printf(" #  In all of the above games                     ****                        #\n");
	printf(" #  Press 'esc' to exit the game                 ***                          #\n");
	printf(" #  Press 'F5' to restart the game              **                            #\n");
	printf(" #                                             *                              #\n");
	printf(" #                                                                            #\n");
	printf(" ==============================================================================\n");    

	printf("\n\n");
}

void       filecmdList()
{
	printf(" ==============================================================================\n");
	printf(" #  *Lightning OS*                                            *   TONGJI UNI  #\n");
	printf(" #                                                           **    kefan.Z    #\n");
	printf(" #  [FS COMMAND LIST]                                        *                #\n");
	printf(" #                                                                            #\n");
	printf(" #  1. ls                     : List all files in current directory           #\n");
	printf(" #                                                                            #\n");
	printf(" #  2. touch    [filename]    : Create a new file in current directory        #\n");
	printf(" #                                                                            #\n");
	printf(" #  3. rm       [filename]    : Delete a file in current directory            #\n");
	printf(" #                                                                            #\n");
	printf(" #  4. rm  -r   [foldername]  : Delete a folder in current directory          #\n");
	printf(" #                                                                            #\n");
	printf(" #  5. rd       [filename]    : Print the content of a file                   #\n");
	printf(" #                                                                            #\n");
	printf(" #  6. wt       [filename]    : Write new content at the end of the file      #\n");
	printf(" #                                                                            #\n");
	printf(" #  7. mkdir    [foldername]  : Create a new directory in current directory   #\n");
	printf(" #                                                                            #\n");
	printf(" #  8. cd       [foldername]  : Go to a directory in current directory        #\n");
	printf(" #                                                                            #\n");
	printf(" ==============================================================================\n");
	printf("\n");
}
