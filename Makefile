###############################################################################
#
# Conquest Makefile
#
###############################################################################

.PHONY: all clean

all:
	@make -C src --no-print-directory
	@echo "All done!"
	
clean:
	@make -C src clean --no-print-directory
	@echo "Cleaned!"

