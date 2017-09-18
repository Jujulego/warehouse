# Variables
BUILD = build/cmake
LIB   = lib
BIN   = bin
SRCS  = src
TPLS  = tpl
EXEC  = warehouse

VALGRIND_OPTS  = --num-callers=500 #--leak-check=full #--show-leak-kinds=all --gen-suppressions=yes
HELGRIND_OPTS  = --num-callers=500 #--gen-suppressions=yes
VALGRIND_SUPPS = $(patsubst %,--suppressions=$(shell pwd)/%,$(wildcard valgrind/*.supp)) --suppressions=/usr/lib/valgrind/python3.supp

EXEC_OPTS  = -v
CMAKE_OPTS =

# Commandes
RM    = rm -rf
CMAKE = /usr/local/bin/cmake
VALGRIND = valgrind

# Options
$(VERBOSE).SILENT:
.NOTPARALLEL:

# Compilation !
all: $(BUILD)
	$(MAKE) -C $(BUILD)

.PHONY: all

rebuild: $(BUILD)
	$(MAKE) -C $(BUILD) clean all

valgrind: all
	$(VALGRIND) $(VALGRIND_OPTS) $(VALGRIND_SUPPS) ./$(EXEC) $(EXEC_OPTS)

.PHONY: valgrind

helgrind: all
	$(VALGRIND) --tool=helgrind $(HELGRIND_OPTS) $(VALGRIND_SUPPS) ./$(EXEC) $(EXEC_OPTS)

.PHONY: helgrind

# Génération graphe d'appel
warehouse.svg: $(BUILD)
	$(MAKE) -C $(BUILD) warehouse.svg

# Analyse par Clang++
analyse: $(BUILD)
	$(MAKE) -C $(BUILD) analyse

# Documentation
docs: $(BUILD)
	$(MAKE) -C $(BUILD) docs
	cp -ru $(BUILD)/docs .

.PHONY: docs

# Nettoyage
clean:
	$(RM) $(BUILD) $(LIB) $(EXEC)

.PHONY: docs

# Dossier $(BUILD)
$(BUILD): CMakeLists.txt $(SRCS) $(TPLS)
	echo "\033[35;01mConstruction du dossier $(BUILD)\033[m"
	[ -d "$(BUILD)" ] || mkdir -p $(BUILD)
	cd $(BUILD); $(CMAKE) $(CMAKE_OPTS) ../..

.PHONY: $(BUILD)
