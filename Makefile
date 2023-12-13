all:
	gcc -g -o user Users/user.c utils/checker.c
	gcc -g -o AS Auction_Server/AS.c Auction_Server/AS_helper.c utils/checker.c

user:
	gcc -g -o user Users/user.c utils/checker.c

server:
	gcc -g -o AS Auction_Server/AS.c Auction_Server/AS_helper.c utils/checker.c

clean:
	rm user
	rm AS
