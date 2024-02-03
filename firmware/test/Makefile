Arguments := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))

.PHONY: test
###############################################################################
# Local Commands
###############################################################################
all: test

init:
	@docker build -t polyglot .

test:
	@docker run --rm -ti -v "./../":"/polyglot" polyglot -t $(Arguments)
