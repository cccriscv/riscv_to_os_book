FILES = metadata.md 00-preface.md 01-OsInterface.md 02-OsOrganization.md  03-PageTables.md \
	04-TrapCalls.md 05-InterruptDirver.md 06-lock.md 07-Scheduling.md \
	08-FileSystem.md 09-Concurrency.md 10-Summary.md
BOOK = xv6book

all: mobi html

mobi: epub
	ebook-convert $(BOOK).epub $(BOOK).mobi

html: $(FILES)
	pandoc --css=book.css -o $(BOOK).html $^

epub: $(FILES)
	pandoc --css=book.css -o $(BOOK).epub $^
	# pandoc --metadata-file=book.yaml -o book.epub $^

clean:
	rm -f *.epub *.mobi
