CC = gcc
CFLAGS = -Wall -Wextra
TARGET = mp3tagreader
SOURCES = main.c id3_reader.c id3_writer.c id3_utils.c error_handling.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe a.out *.tmp *.bak
