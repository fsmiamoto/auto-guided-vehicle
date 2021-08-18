# Settings
LANG  := pt-BR
MARGIN := 0.8in

IMG   := $(wildcard img/*)

PFLAGS := -V lang=$(LANG) -V geometry:margin=$(MARGIN)

domain: $(IMG)
	pandoc $(PFLAGS) domain.md -o ../requisitos/dominio.pdf

arch: $(IMG)
	pandoc $(PFLAGS) architecture.md -o ../arquitetura/arquitetura.pdf

clean:
	$(RM) $(TARGET)

.PHONY: clean arch domain
