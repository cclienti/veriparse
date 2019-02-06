# Common Makefile parts
# Copyright (C) 2013 Christophe Clienti - All Rights Reserved

print-%:
	@echo $* = $($*)

clean::
	rm -rf *~ *# ../src/*~ ../src/*#
