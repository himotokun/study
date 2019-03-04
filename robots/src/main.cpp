#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "lexlib.h"
#include "syntax.h"
#include "game.h"
#include "errors.h"

GameClass Game;

int main(int argc, char **argv)
{
	if(argc >= 4){
		int fd = open(argv[3], O_RDONLY);
		if(fd == -1){
			perror(argv[3]);
			exit(1);
		}
		dup2(fd, 0);
		close(fd);
	}else if(argc < 3){
		printf("usage: r <ip> <port> [file [nick] ]\n");
		exit(1);
	}
	LexicalAnalyzer LA;
	int c;
	while((c = getchar()) != EOF){
		try{
			LA.Push(c);
		}
		catch(lexem& l){
			printf("unknown lexem %s at string %d\n", l.data, l.string);
			return 1;
		}
	}
	SyntaxAnalyzer SA(LA.GetList());
	try{
		SA.Check();
		Game.Open(argv[1], atoi(argv[2]));
		Game.Start(argc == 5 ? argv[4] : "bot");
		SA.Run();
		Game.Close();
	}
	catch(SyntaxException& e){
		printf("%s at lexem \"%s\" (string %d)\n",
				e.Type(), e.Lexem(), e.String());
		return 1;
	}
	return 0;
}
