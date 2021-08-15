# Settings
TARGET := result.pdf
LANG  := pt-BR

SRC   := $(wildcard *.md)
IMG   := $(wildcard img/*)

PFLAGS := -V lang=$(LANG) -V geometry:margin=0.8in

$(TARGET): $(SRC) $(IMG)
	pandoc $(PFLAGS) $(SRC) -o $(TARGET)

clean:
	$(RM) $(TARGET)

.PHONY: clean
