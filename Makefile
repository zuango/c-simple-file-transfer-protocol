
# ----------------------------------------
# COMPILER SETTINGS
# ----------------------------------------
CC      := gcc
CFLAGS  := -std=c17 -Wall -Wextra -Werror -O2
LDFLAGS :=

# Output directory
BUILD   := build

# ----------------------------------------
# SOURCE FILES
# ----------------------------------------
SERVER_SRC := \
    server/server.c \
    server/handler.c \
    shared/net.c

CLIENT_SRC := \
    client/client.c \
    client/sendfile.c \
    shared/net.c

# Final binaries
SERVER_BIN := $(BUILD)/server
CLIENT_BIN := $(BUILD)/client

# ----------------------------------------
# BUILD RULES
# ----------------------------------------
all: $(SERVER_BIN) $(CLIENT_BIN)

# --- Build server ---
$(SERVER_BIN): $(SERVER_SRC)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRC) $(LDFLAGS)
	@echo "[OK] Built server"

# --- Build client ---
$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC) $(LDFLAGS)
	@echo "[OK] Built client"

# ----------------------------------------
# PHONY TARGETS
# ----------------------------------------
clean:
	rm -rf $(BUILD)
	@echo "[CLEAN] build directory removed"

run-server: $(SERVER_BIN)
	./$(SERVER_BIN)

run-client: $(CLIENT_BIN)
	./$(CLIENT_BIN)

.PHONY: all clean run-server run-client
