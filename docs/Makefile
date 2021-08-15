# Settings
TARGET := result.pdf
LANG  := pt-BR

SRC   := $(wildcard *.md)
IMG   := $(wildcard img/*)

PFLAGS := -V lang=$(LANG)

$(TARGET): $(SRC) $(IMG)
	pandoc $(PFLAGS) $(SRC) -o $(TARGET)

clean:
	$(RM) $(TARGET)

.PHONY: clean
