all:
	(cd Users; make all)
	(cd Auction_Server; make all)

clean:
	(cd Users; make clean)
	(cd Auction_Server; make clean)
